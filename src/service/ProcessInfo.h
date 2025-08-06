#pragma once

#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    std::string user;
    std::string state;
    std::string command;
    float cpu_usage = 0.0f;
    float memory_usage = 0.0f;
    float memory_percent = 0.0f;
    int thread_count = 0;

    enum SortField { PID, NAME, USER, CPU, MEMORY, COMMAND };
    static void sortProcesses(std::vector<ProcessInfo> &processes, SortField field, bool ascending);
};


