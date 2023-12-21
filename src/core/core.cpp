#include "core/core.h"

Core::Core()
{
    const uint32_t num_threads = std::thread::hardware_concurrency() - 1;
    for (uint32_t i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(std::thread(&Core::threadLoop, this));
    }
}

void Core::queueJob(const std::function<void()> &job)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

bool Core::isBusy()
{
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = !jobs.empty() || inProgress != 0;
    }
    return poolbusy;
}

void Core::stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}

void Core::threadLoop()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this]
                                 { return !jobs.empty() || should_terminate; });
            if (should_terminate)
            {
                return;
            }
            inProgress++;
            job = jobs.front();
            jobs.pop();
        }
        job();
        inProgress--;
    }
}