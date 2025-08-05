#ifdef _WIN32

#include "ProcessInfo.h"
#include <optional>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>

#include "ProcessService.h"

std::vector<ProcessInfo> ProcessService::list_all_processes() {
    std::vector<ProcessInfo> processes;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return processes;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        do {
            ProcessInfo info = {};
            info.pid = entry.th32ProcessID;
            info.ppid = entry.th32ParentProcessID;
            info.name = entry.szExeFile;
            info.cmdline = "";
            info.user = "";
            info.state = "Unknown";
            info.cpu_usage = 0.0f;
            info.memory_usage = 0.0f;
            info.memory_percent = 0.0f;
            info.thread_count = entry.cntThreads;

            processes.push_back(info);
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return processes;
}

std::optional<ProcessInfo> ProcessService::get_process_info(int pid) {
    for (const auto& p : list_all_processes()) {
        if (p.pid == pid) return p;
    }
    return std::nullopt;
}

#endif