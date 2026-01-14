#include <chrono>
#include <climits>
#include <print>
#include <thread>

#include "logger.hpp"
#include "task_dispatcher.hpp"

using namespace dispatcher;

int main() {
    std::map<TaskPriority, queue::QueueOptions> m {
        { TaskPriority::High, { true, 1000 } },
        { TaskPriority::Normal, { false, std::nullopt } }        
    };

    TaskDispatcher td(std::thread::hardware_concurrency(), m);
    std::vector<std::jthread> threads;

    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < 10; j++) {
                td.schedule(TaskPriority::Normal,
                            [=]() { Logger::Get().Log("Normal priority message №" + std::to_string(10 * i + j)); });
                td.schedule(TaskPriority::High,
                            [=]() { Logger::Get().Log("High priority message №" + std::to_string(10 * i + j)); });
            }
        });
    }
    return 0;
}
