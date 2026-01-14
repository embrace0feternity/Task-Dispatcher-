#include "task_dispatcher.hpp"

namespace dispatcher {

namespace details {

std::map<TaskPriority, queue::QueueOptions> createDefaultConfig() noexcept {
    return {
        { TaskPriority::High, { true, 1000 } },
        { TaskPriority::Normal, { false, std::nullopt } }        
    };   
}

}

///
///
///

TaskDispatcher::TaskDispatcher( 
    size_t thread_count, std::map<TaskPriority, queue::QueueOptions> config) :
    pool(std::make_shared<queue::PriorityQueue>(config), thread_count) 
{}

void TaskDispatcher::schedule(TaskPriority priority, Task task) {
    pool.runTask(priority, task);
}

} // namespace dispatcher