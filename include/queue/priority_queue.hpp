#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <ranges>
#include <algorithm>

namespace dispatcher::queue {

namespace details {

std::unique_ptr<IQueue> createUnboundQueue() noexcept;

std::unique_ptr<IQueue> createBoundQueue(int capacity) noexcept;

} // namespace details

///
///
///

class PriorityQueue {
public:
    using Task = std::function<void()>;

public:
    explicit PriorityQueue(const std::map<TaskPriority, QueueOptions> &qConfiguration):
        qs {} 
    {
        std::ranges::for_each(qConfiguration, [&](const auto &pair){
            auto [priority, config] = pair;
            if (config.bounded) {
                qs.emplace(priority, details::createBoundQueue(config.capacity.value_or(1000)));
            }
            else
            {
                qs.emplace(priority, details::createUnboundQueue());
            }
        });
    }

    void push(TaskPriority priority, Task task);

    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<Task> pop();

    void shutdown();

private:
    std::map<TaskPriority, std::unique_ptr<IQueue>> qs;
    std::atomic<int> taskNumber = 0;
    std::condition_variable rx;
    std::mutex m;
    bool isActive = true;
};

}  // namespace dispatcher::queue