#pragma once

#include <memory>
#include <thread>
#include <vector>

#include "queue/priority_queue.hpp"

namespace dispatcher::thread_pool {

class ThreadPool {
    void work();

public:
    using Task = queue::PriorityQueue::Task;

public:
    explicit ThreadPool(
        std::shared_ptr<queue::PriorityQueue> pq, 
        std::size_t threadNumber
    );

    void runTask(TaskPriority priority, Task task);

    ~ThreadPool();

private:
    std::shared_ptr<queue::PriorityQueue> pq;
    std::vector<std::jthread> workers;
    std::atomic_bool stopWorking;
};

} // namespace dispatcher::thread_pool
