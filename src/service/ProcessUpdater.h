#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

#include "ProcessInfo.h"

class ProcessUpdater {
public:
    ProcessUpdater(std::shared_ptr<std::vector<ProcessInfo>> *shared_ptr_var);

    ~ProcessUpdater();
    void start();
    void stop();
    void set_update_interval(float seconds);

private:
    void update_loop();
    std::shared_ptr<std::vector<ProcessInfo>> *shared_ptr_var;

    int counter = 0;

    std::thread worker_thread;
    std::atomic<bool> running{ false };
    std::condition_variable cv;
    std::mutex cv_mutex;
    float update_interval_seconds = 5.0f;
};
