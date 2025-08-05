#include "ProcessUpdater.h"
#include <chrono>
#include <iostream>
#include <stdexcept>

#include "ProcessService.h"

ProcessUpdater::ProcessUpdater(std::shared_ptr<std::vector<ProcessInfo>> *shared_ptr_var)
    : shared_ptr_var(shared_ptr_var) {
}

ProcessUpdater::~ProcessUpdater() {
    stop();
}

void ProcessUpdater::start() {
    if (running) return;
    running = true;
    worker_thread = std::thread(&ProcessUpdater::update_loop, this);
}

void ProcessUpdater::stop() {
    if (!running) return;
    running = false;
    cv.notify_all();
    if (worker_thread.joinable())
        worker_thread.join();
}

void ProcessUpdater::set_update_interval(float seconds) {
    update_interval_seconds = seconds;
}

void ProcessUpdater::update_loop() {
    while (running) {
        auto processes = ProcessService::list_all_processes();

        std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            float score_a = a.cpu_usage * 0.7f + a.memory_percent * 0.3f;
            float score_b = b.cpu_usage * 0.7f + b.memory_percent * 0.3f;
            return score_a > score_b;
        });

        auto updated_process_list = std::make_shared<std::vector<ProcessInfo>>(processes);

        std::atomic_store(shared_ptr_var, updated_process_list);

        counter++;
        std::unique_lock<std::mutex> lock(cv_mutex);
        cv.wait_for(lock, std::chrono::duration<float>(update_interval_seconds), [this]() { return !running.load(); });
    }
}
