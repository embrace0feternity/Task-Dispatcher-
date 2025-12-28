#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

#include "queue/queue.hpp"

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
public:
    using Task = typename IQueue::Task;

public:
    explicit BoundedQueue(int capacity);

    /// blocked method
    void push(Task task) override;

    /// non-blocked method
    std::optional<Task> try_pop() override;

private:
    std::queue<Task> q;
    std::mutex m;
    std::condition_variable tx;
    int capacity;
};

}  // namespace dispatcher::queue