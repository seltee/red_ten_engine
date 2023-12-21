#pragma once
#include "common/utils.h"
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <queue>

class Core
{
public:
    Core();

    void queueJob(const std::function<void()> &job);
    bool isBusy();

private:
    void stop();
    void threadLoop();

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;

    std::atomic<int> inProgress = 0;
};
