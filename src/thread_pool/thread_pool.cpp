#include "thread_pool/thread_pool.hpp"

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(
    std::shared_ptr<queue::PriorityQueue> pq, 
    std::size_t threadNumber) :
    pq { pq }, 
    workers {},
    stopWorking { false }
{
    workers.reserve(threadNumber);
    for (int i = 0; i < threadNumber; ++i) {
        workers.emplace_back(&ThreadPool::work, this);
    }
} 

void ThreadPool::work() {
    while (true) {
        /// Work until the q isn't empty
        ///  and shutdown hasn't been called
        auto task = pq->pop();
        if (task.has_value()) {
            task.value()();
        } else {
            if (stopWorking) {
                break;
            }
            std::this_thread::yield();
        } 
    }
}

void ThreadPool::runTask(TaskPriority priority, Task task) {
    pq->push(priority, task);
}

ThreadPool::~ThreadPool() {
    stopWorking = true;
    /// jthread join call
    pq->shutdown();
}

} // namespace dispatcher::thread_pool