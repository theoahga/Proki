// Process.h
#pragma once
#include "ProcessInfo.h"
#include <vector>
#include <optional>

class ProcessService {
public:
    static std::vector<ProcessInfo> list_all_processes();
    static std::optional<ProcessInfo> get_process_info(int pid);
    static unsigned long long get_total_system_cpu_time();
    static unsigned long long get_process_cpu_time(int pid);
};
