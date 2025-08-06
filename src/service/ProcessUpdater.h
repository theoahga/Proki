#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#include "ProcessInfo.h"

class ProcessUpdater {
public:
    ProcessUpdater();
    ~ProcessUpdater();

    void start();
    void stop();

    std::vector<ProcessInfo> get_processes();

private:
    void update_loop();
    std::shared_ptr<std::vector<ProcessInfo>> *shared_ptr_var;

    std::atomic<bool> m_IsRunning{false};
    std::thread m_WorkerThread;
    std::mutex m_DataMutex;

    std::vector<ProcessInfo> m_Processes;

    std::map<int, unsigned long long> m_PrevProcessCpuTimes;
    unsigned long long m_PrevTotalSystemCpuTime = 0;
};
