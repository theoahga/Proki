#if defined(__APPLE__)

#include "ProcessInfo.h"
#include <sys/sysctl.h>
#include <libproc.h>
#include <pwd.h>

#include "ProcessService.h"

std::vector<ProcessInfo> ProcessService::list_all_processes() {
    std::vector<ProcessInfo> processes;

    int num_pids = proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0) / sizeof(pid_t);
    std::vector<pid_t> pids(num_pids);
    proc_listpids(PROC_ALL_PIDS, 0, pids.data(), num_pids * sizeof(pid_t));

    for (pid_t pid : pids) {
        if (pid <= 0) continue;

        ProcessInfo info = {};
        info.pid = pid;

        struct proc_bsdinfo bsd;
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsd, sizeof(bsd)) > 0) {
            info.ppid = bsd.pbi_ppid;
            info.name = bsd.pbi_name;
            info.thread_count = bsd.pbi_nfiles;

            uid_t uid = bsd.pbi_uid;
            struct passwd *pw = getpwuid(uid);
            info.user = pw ? pw->pw_name : "unknown";

            info.state = "Unknown";
            info.cpu_usage = 0.0f;
            info.memory_usage = 0.0f;
            info.memory_percent = 0.0f;
        }

        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        if (proc_pidpath(pid, pathbuf, sizeof(pathbuf)) > 0) {
            info.cmdline = pathbuf;
        }

        processes.push_back(info);
    }

    return processes;
}

std::optional<ProcessInfo> ProcessService::get_process_info(int pid) {
    for (const auto& p : list_all_processes()) {
        if (p.pid == pid) return p;
    }
    return std::nullopt;
}

#endif
