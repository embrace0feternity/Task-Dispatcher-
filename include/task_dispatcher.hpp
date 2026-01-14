#pragma once

#include <memory>

#include "queue/priority_queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {


namespace details {

std::map<TaskPriority, queue::QueueOptions> createDefaultConfig() noexcept;

} // namespace details

///
///
///

class TaskDispatcher {
public:
    using Task = thread_pool::ThreadPool::Task;
public:
    TaskDispatcher(
        size_t thread_count = std::thread::hardware_concurrency(), 
        std::map<TaskPriority, queue::QueueOptions> config = details::createDefaultConfig()
    );

    void schedule(TaskPriority priority, Task task);

private:
    thread_pool::ThreadPool pool;
};

}  // namespace dispatcher