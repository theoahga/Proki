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
        auto updated_process_list = std::make_shared<std::vector<ProcessInfo>>(ProcessService::list_all_processes());
        std::atomic_store(shared_ptr_var, updated_process_list);

        counter++;
        std::unique_lock<std::mutex> lock(cv_mutex);
        cv.wait_for(lock, std::chrono::duration<float>(update_interval_seconds), [this]() { return !running.load(); });
    }
}
