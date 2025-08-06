#include "ProcessInfo.h"

#include <algorithm>

void ProcessInfo::sortProcesses(std::vector<ProcessInfo> &processes, ProcessInfo::SortField field, bool ascending = true) {
    auto comparator = [&](const ProcessInfo &a, const ProcessInfo &b) {
        switch (field) {
            case ProcessInfo::PID:
                return ascending ? a.pid<b.pid : a.pid> b.pid;
            case ProcessInfo::NAME:
                return ascending ? a.name<b.name : a.name> b.name;
            case ProcessInfo::USER:
                return ascending ? a.user<b.user : a.user> b.user;
            case ProcessInfo::CPU:
                return ascending ? a.cpu_usage<b.cpu_usage : a.cpu_usage> b.cpu_usage;
            case ProcessInfo::MEMORY:
                return ascending ? a.memory_usage<b.memory_usage : a.memory_usage> b.memory_usage;
            case ProcessInfo::THREADS:
                return ascending ? a.thread_count<b.thread_count : a.thread_count> b.thread_count;
            default:
                return true;
        }
    };

    std::ranges::sort(processes, comparator);
}
