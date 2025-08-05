#if !defined(_WIN32) && !defined(__APPLE__)

#include "ProcessInfo.h"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <pwd.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include "ProcessService.h"

static std::string read_file(const std::string& path) {
    std::ifstream file(path);
    return file ? std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()) : "";
}

static std::string get_user_from_uid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

static ProcessInfo parse_process(int pid) {
    ProcessInfo info = {};
    info.pid = pid;

    std::string status_path = "/proc/" + std::to_string(pid) + "/status";
    std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
    std::string cmdline_path = "/proc/" + std::to_string(pid) + "/cmdline";

    std::ifstream status(status_path);
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
            info.memory_usage = std::stof(line.substr(7)) / 1024.0f;  // kB to MB
    }

    std::ifstream cmdfile(cmdline_path);
    std::getline(cmdfile, info.cmdline, '\0');

    // Simplified placeholders for CPU/mem percent
    info.cpu_usage = 0.0f;
    info.memory_percent = 0.0f;

    return info;
}

std::vector<ProcessInfo> ProcessService::list_all_processes() {
    std::vector<ProcessInfo> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            int pid = atoi(entry->d_name);
            if (pid > 0) {
                try {
                    processes.push_back(parse_process(pid));
                } catch (...) {}
            }
        }
    }
    closedir(dir);
    return processes;
}

std::optional<ProcessInfo> ProcessService::get_process_info(int pid) {
    try {
        return parse_process(pid);
    } catch (...) {
        return std::nullopt;
    }
}

#endif
