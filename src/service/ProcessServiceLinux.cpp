#if !defined(_WIN32) && !defined(__APPLE__)

#include "ProcessInfo.h"
#include "ProcessService.h"

#include <fstream>
#include <sstream>
#include <dirent.h>
#include <pwd.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <optional>
#include <algorithm>

static std::string get_user_from_uid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

static long get_total_memory_kb() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.rfind("MemTotal:", 0) == 0) {
            std::istringstream iss(line.substr(9));
            long mem_kb;
            iss >> mem_kb;
            return mem_kb;
        }
    }
    return 0;
}

static long long get_total_cpu_time() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line.substr(5)); // Skip "cpu  "
    long long total = 0, val;
    while (iss >> val)
        total += val;
    return total;
}

static long long get_process_cpu_time(int pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file.is_open()) return 0;
    std::string token;
    for (int i = 0; i < 13; ++i) file >> token;
    long utime, stime;
    file >> utime >> stime;
    return static_cast<long long>(utime + stime);
}

static std::string read_cmdline(int pid) {
    std::ifstream cmdfile("/proc/" + std::to_string(pid) + "/cmdline");
    std::string cmdline;
    std::getline(cmdfile, cmdline, '\0');
    std::replace(cmdline.begin(), cmdline.end(), '\0', ' ');
    return cmdline;
}

static ProcessInfo parse_process(int pid, long total_mem_kb) {
    ProcessInfo info = {};
    info.pid = pid;

    std::ifstream status("/proc/" + std::to_string(pid) + "/status");
    if (!status.is_open()) throw std::runtime_error("status not readable");

    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind("Name:", 0) == 0)
            info.name = line.substr(6);
        else if (line.rfind("Uid:", 0) == 0)
            info.user = get_user_from_uid(std::stoi(line.substr(5)));
        else if (line.rfind("State:", 0) == 0)
            info.state = line.substr(7);
        else if (line.rfind("PPid:", 0) == 0)
            info.ppid = std::stoi(line.substr(6));
        else if (line.rfind("Threads:", 0) == 0)
            info.thread_count = std::stoi(line.substr(9));
        else if (line.rfind("VmRSS:", 0) == 0)
            info.memory_usage = std::stof(line.substr(7)) / 1024.0f; // kB to MB
    }

    info.memory_percent = (total_mem_kb > 0)
        ? (info.memory_usage * 1024.0f / total_mem_kb) * 100.0f
        : 0.0f;

    info.cmdline = read_cmdline(pid);

    int cpu_cores = std::max(1L, sysconf(_SC_NPROCESSORS_ONLN));
    info.cpu_usage = 0.0f;

    return info;
}

std::vector<ProcessInfo> ProcessService::list_all_processes() {
    std::vector<ProcessInfo> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    long total_mem_kb = get_total_memory_kb();

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            const char* name = entry->d_name;
            if (std::all_of(name, name + std::strlen(name), ::isdigit)) {
                int pid = std::atoi(name);
                try {
                    processes.push_back(parse_process(pid, total_mem_kb));
                } catch (...) {
                    // Ignore processes we can't read
                }
            }
        }
    }
    closedir(dir);
    return processes;
}

std::optional<ProcessInfo> ProcessService::get_process_info(int pid) {
    try {
        return parse_process(pid, get_total_memory_kb());
    } catch (...) {
        return std::nullopt;
    }
}

#endif
