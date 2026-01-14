#pragma once
#include <queue>
#include <mutex>

#include "queue/queue.hpp"

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
public:
    using Task = typename IQueue::Task;

public:
    /// blocked method
    void push(Task task) override;
    
    /// non-blocked method
    std::optional<Task> try_pop() override;

private:
    std::queue<Task> q;
    std::mutex m;
};

}  // namespace dispatcher::queue