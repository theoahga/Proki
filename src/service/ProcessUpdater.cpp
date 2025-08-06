#include "ProcessUpdater.h"
#include <chrono>
#include <iostream>
#include <stdexcept>

#include "ProcessService.h"

ProcessUpdater::ProcessUpdater(): shared_ptr_var(nullptr) {
    m_PrevTotalSystemCpuTime = ProcessService::get_total_system_cpu_time();
    auto processes = ProcessService::list_all_processes();
    for (const auto &p: processes) {
        m_PrevProcessCpuTimes[p.pid] = ProcessService::get_process_cpu_time(p.pid);
    }
}

ProcessUpdater::~ProcessUpdater() {
    stop();
}

void ProcessUpdater::start() {
    if (m_IsRunning) return;
    m_IsRunning = true;
    m_WorkerThread = std::thread(&ProcessUpdater::update_loop, this);
}

void ProcessUpdater::stop() {
    if (!m_IsRunning) return;
    m_IsRunning = false;
    if (m_WorkerThread.joinable())
        m_WorkerThread.join();
}

void ProcessUpdater::update_loop() {
    while (m_IsRunning) {
        // Wait a second
        std::this_thread::sleep_for(std::chrono::seconds(1));

        unsigned long long currentTotalSystemTime = ProcessService::get_total_system_cpu_time();
        auto newProcesses = ProcessService::list_all_processes();
        std::map<int, unsigned long long> currentProcessCpuTimes;

        unsigned long long systemTimeDiff = currentTotalSystemTime - m_PrevTotalSystemCpuTime;

        for (auto& p : newProcesses) {
            unsigned long long currentProcessTime = ProcessService::get_process_cpu_time(p.pid);
            currentProcessCpuTimes[p.pid] = currentProcessTime;

            auto it = m_PrevProcessCpuTimes.find(p.pid);
            if (it != m_PrevProcessCpuTimes.end() && systemTimeDiff > 0) {
                unsigned long long processTimeDiff = currentProcessTime - it->second;
                p.cpu_usage = 100.0f * static_cast<float>(processTimeDiff) / static_cast<float>(systemTimeDiff);
            }
        }

        ProcessInfo::sortProcesses(newProcesses, ProcessInfo::MEMORY, false);

        // Update the shared m_Processes std::vector
        {
            std::lock_guard<std::mutex> lock(m_DataMutex);
            m_Processes = std::move(newProcesses);
        }

        // Save current values for the next iteration
        m_PrevTotalSystemCpuTime = currentTotalSystemTime;
        m_PrevProcessCpuTimes = std::move(currentProcessCpuTimes);

    }
}

std::vector<ProcessInfo> ProcessUpdater::get_processes() {
    std::lock_guard<std::mutex> lock(m_DataMutex);
    return m_Processes;
}