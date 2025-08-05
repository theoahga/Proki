// Process.h
#pragma once
#include "ProcessInfo.h"
#include <vector>
#include <optional>

class ProcessService {
public:
    static std::vector<ProcessInfo> list_all_processes();
    static std::optional<ProcessInfo> get_process_info(int pid);
};
