#include <gtest/gtest.h>

#include <thread>
#include <chrono>

#include "queue/priority_queue.hpp"

using namespace std::chrono_literals;
using namespace dispatcher::queue;
using namespace dispatcher;

class PQTestGroup : public testing::Test {
protected:
    using Task = typename PriorityQueue::Task;

    int highPriorityCallNumber = 0;
    int normalPriorityCallNumber = 0;

    Task highPriorityTask = [&](){
        highPriorityCallNumber += 1;
    };

    Task normalPriorityTask = [&](){
        normalPriorityCallNumber += 1;
    };
};

///
///
///

TEST_F(PQTestGroup, PushMultiThread) {
    {
        std::map<TaskPriority, queue::QueueOptions> m {
            { TaskPriority::High, { true, 10 } },
            { TaskPriority::Normal, { false, std::nullopt } }        
        };

        PriorityQueue pq { m };
    
        std::atomic<int> counter = 0;

        {
            std::jthread th1 { [&]{
                for (int i = 0; i < 5; ++i) {
                    pq.push(TaskPriority::High, [](){});
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            }}; 

            std::jthread th2 { [&]{
                for (int i = 0; i < 25; ++i) {
                    pq.push(TaskPriority::Normal, [](){});
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            }}; 
        }

        EXPECT_EQ(30, counter);
    }
}

///
///
///

TEST_F(PQTestGroup, PushPopMultiThread) {
    {
        std::map<TaskPriority, queue::QueueOptions> m {
            { TaskPriority::High, { true, 10 } },
            { TaskPriority::Normal, { false, 10 } }        
        };

        PriorityQueue pq { m };
    
        std::atomic<int> pushCounter = 0;
        std::atomic<int> popCounter = 0;

        {
            std::jthread th1 { [&]{
                for (int i = 0; i < 5; ++i) {
                    pq.push(TaskPriority::High, [](){});
                    pushCounter.fetch_add(1, std::memory_order_relaxed);
                }
            }}; 

            std::jthread th2 { [&]{
                for (int i = 0; i < 25; ++i) {
                    pq.push(TaskPriority::Normal, [](){});
                    pushCounter.fetch_add(1, std::memory_order_relaxed);
                }
            }};

            std::jthread th3 { [&]{
                while (popCounter != 30) {
                    auto rt = pq.pop();
                    if (rt.has_value()) {
                        popCounter.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }};
        }

        EXPECT_EQ(popCounter, pushCounter);

        /// Pop is a blocked method
        ///  if code is still running, the test is passed :)

        /// Block and release code
        {
            bool hasValue;
            std::jthread th4 { [&]{
                hasValue = pq.pop().has_value();
            }};

            std::jthread th5 { [&]{
                std::this_thread::sleep_for(1ms);
                pq.shutdown();
            }};

            EXPECT_FALSE(hasValue);
        }
    }
}

///
///
///

TEST_F(PQTestGroup, CheckPriority) {
    {
        std::map<TaskPriority, queue::QueueOptions> m {
            { TaskPriority::Normal, { true, 10 } },
            { TaskPriority::High, { true, std::nullopt } }        
        };

        PriorityQueue pq { m };
        int hp = 25;
        int np = 5;
    
        for (int i = 0; i < np; ++i) {
            pq.push(TaskPriority::Normal, normalPriorityTask);
        }
        for (int i = 0; i < hp; ++i) {
            pq.push(TaskPriority::High, highPriorityTask);
        }

        bool hasValue = true;
        for (int i = 0; i < hp; ++i) {
            auto rt = pq.pop();
            hasValue &= rt.has_value();
            if (rt.has_value()) {
                rt.value()();
            }
        }

        EXPECT_EQ(hp, highPriorityCallNumber);
        EXPECT_TRUE(hasValue);

        auto rt = pq.pop();
        EXPECT_TRUE(rt.has_value());
        rt.value()();
        EXPECT_EQ(1, normalPriorityCallNumber);
    }
}