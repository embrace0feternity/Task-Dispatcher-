#include "queue/priority_queue.hpp"

#include <thread>

namespace dispatcher::queue {

namespace details {

std::unique_ptr<IQueue> createUnboundQueue() noexcept {
    return std::make_unique<UnboundedQueue>();
}

std::unique_ptr<IQueue> createBoundQueue(int capacity) noexcept {
    return std::make_unique<BoundedQueue>(capacity);
}

}

///
///
///

void PriorityQueue::push(TaskPriority priority, Task task) {
    qs[priority]->push(task);
    taskNumber.fetch_add(1, std::memory_order_release);
    rx.notify_all();
}    

std::optional<PriorityQueue::Task> PriorityQueue::pop() {
    std::unique_lock lk { m };
    
    rx.wait(lk, [&](){ return taskNumber.load() > 0 || !isActive; });

    if (!isActive && (taskNumber.load() == 0)) {
        return std::nullopt;
    }

    auto rt = qs[TaskPriority::High]->try_pop();
    if (rt.has_value()) {
        taskNumber.fetch_sub(1, std::memory_order_release);
        return rt;
    }
    
    rt = qs[TaskPriority::Normal]->try_pop();
    if (rt.has_value()) {
        taskNumber.fetch_sub(1, std::memory_order_release);
    }
    return rt;
}

void PriorityQueue::shutdown() {
    std::lock_guard lk { m };
    isActive = false;
    rx.notify_all();
}

} // namespace dispatcher::queue