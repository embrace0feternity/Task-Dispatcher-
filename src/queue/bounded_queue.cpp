#include "queue/bounded_queue.hpp"

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(int capacity) : IQueue(), 
    q {}, 
    m {},
    tx {},
    capacity { capacity } {}

void BoundedQueue::push(Task task) {
    std::unique_lock lk(m);
    tx.wait(lk, [&](){ return q.size() < capacity; });
    
    q.push(std::move(task));
}

std::optional<BoundedQueue::Task> BoundedQueue::try_pop() {
    std::optional<Task> rt = std::nullopt;
    {
        std::unique_lock lk(m);
        if (q.empty()) {
            return std::nullopt;
        }

        rt = std::move(q.front());
        q.pop();
    }
    tx.notify_one();

    return rt;
}

} // namespace dispatcher::queue