#include "queue/unbounded_queue.hpp"

namespace dispatcher::queue {

void UnboundedQueue::push(Task task) {
    std::unique_lock lk(m);
    q.push(std::move(task));
}

std::optional<UnboundedQueue::Task> UnboundedQueue::try_pop() {
    std::unique_lock lk(m);

    if (q.empty()) {
        return std::nullopt;
    }

    std::optional<Task> rt = std::move(q.front());
    q.pop();

    return rt;
}

} // namespace dispatcher::queue