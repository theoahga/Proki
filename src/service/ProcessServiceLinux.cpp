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
#include <thread>
#include <chrono>
#include <algorithm>

static std::string get_user_from_uid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

unsigned long long ProcessService::get_total_system_cpu_time() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line.substr(5));
    unsigned long long total = 0, val;
    while (iss >> val) total += val;
    return total;
}

unsigned long long ProcessService::get_process_cpu_time(int pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file.is_open()) return 0;
    std::string token;
    for (int i = 0; i < 13; ++i) file >> token;
    unsigned long long utime, stime;
    file >> utime >> stime;
    return utime + stime;
}

unsigned long get_total_memory_kb() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string label, unit;
            unsigned long mem_kb;
            iss >> label >> mem_kb >> unit; // label = "MemTotal:", unit = "kB"
            return mem_kb;
        }
    }
    return 0; // error case
}

std::string get_cmd_line(int pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/cmdline", std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::string line((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::replace(line.begin(), line.end(), '\0', ' ');
    return line;
}

std::vector<ProcessInfo> ProcessService::list_all_processes() {
    std::vector<ProcessInfo> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            const char *name = entry->d_name;
            if (std::all_of(name, name + std::strlen(name), ::isdigit)) {
                int pid = std::atoi(name);
                try {
                    ProcessInfo info = {};
                    info.pid = pid;

                    std::ifstream status("/proc/" + std::to_string(pid) + "/status");
                    if (!status.is_open()) continue;

                    std::string line;
                    while (std::getline(status, line)) {
                        if (line.rfind("Name:", 0) == 0)
                            info.name = line.substr(6);
                        else if (line.rfind("Uid:", 0) == 0)
                            info.user = get_user_from_uid(std::stoi(line.substr(5)));
                        else if (line.rfind("State:", 0) == 0)
                            info.state = line.substr(7);
                        else if (line.rfind("Threads:", 0) == 0)
                            info.thread_count = std::stoi(line.substr(9));
                        else if (line.rfind("VmRSS:", 0) == 0)
                            info.memory_usage = std::stof(line.substr(7));
                    }

                    info.command = get_cmd_line(pid);
                    processes.push_back(info);
                } catch (...) {
                    // Ignore processes we can't read
                }
            }
        }
    }
    closedir(dir);

    auto total_memory_kb = get_total_memory_kb();
    for (auto &p: processes) {
        p.memory_percent = 100.0f * p.memory_usage / total_memory_kb;
    }

    return processes;
}

#endif
