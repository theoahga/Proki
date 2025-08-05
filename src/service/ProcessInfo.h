#pragma once
#include <string>

struct ProcessInfo {
    int pid;                    // Process ID
    int ppid;                   // Parent Process ID
    std::string name;           // Process name or executable
    std::string cmdline;        // Full command line
    std::string user;           // User/owner of the process
    std::string state;          // Running/Sleeping/etc. if available

    float cpu_usage;            // CPU usage in percent
    float memory_usage;         // Memory usage in MB
    float memory_percent;       // RAM usage as percent of total RAM

    int thread_count;           // Number of threads
};