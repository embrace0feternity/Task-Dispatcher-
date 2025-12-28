#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "queue/bounded_queue.hpp"

using namespace std::chrono_literals;
using namespace dispatcher::queue;
using namespace dispatcher;

class BQTestGroup : public testing::Test {
protected:
    using Task = typename BoundedQueue::Task;
    
    int value1 = 0;
    int value2 = 0;

    Task t1 = [&](){
        value1 += 1;
    };
    Task t2 = [&](){
        value2 += 2;
    };
};


///
///
///

TEST_F(BQTestGroup, PushPopSingleThread) {
    BoundedQueue bq { 2 };
    EXPECT_FALSE(bq.try_pop().has_value());

    bq.push(t1);
    bq.push(t2);

    auto rt = bq.try_pop();
    EXPECT_TRUE(rt.has_value());
    rt.value()();
    EXPECT_EQ(1, value1);

    rt = bq.try_pop();
    EXPECT_TRUE(rt.has_value());
    rt.value()();
    EXPECT_EQ(2, value2);
}

///
///
///

TEST_F(BQTestGroup, PushMultiThread) {
    bool isWorking = false;
    int capacity = 1000;
    std::atomic<int> counter = 0;
    BoundedQueue bq { capacity };

    auto push = [&]{
        for (int i = 0; i < (capacity + 1); ++i) {
            bq.push(t1);
            counter.fetch_add(1, std::memory_order::release);
        }
    };

    auto check = [&]{
        std::this_thread::sleep_for(100ms);
        isWorking = counter == capacity;
        std::this_thread::sleep_for(10ms);
        isWorking &= counter == capacity;
        
        /// Make th1 finished
        bq.try_pop();
    };

    {
        std::jthread th1 { push };
        std::jthread th2 { check };
    }
    EXPECT_TRUE(isWorking);
    EXPECT_EQ(capacity + 1, counter);
}

///
///
///

TEST_F(BQTestGroup, PopMultiThread) {
    std::atomic<int> popCount = 0;
    
    int capacity = 100;
    BoundedQueue bq { capacity };

    auto push = [&](){
        for (int i = 0; i < capacity; ++i) {
            bq.push(t1);
        }
    };

    auto pop = [&](){
        while (true) {
            auto rt = bq.try_pop();
            if (!rt.has_value()) {
                break;
            }
            popCount.fetch_add(1, std::memory_order_relaxed);
        }
    };
    
    {
        std::jthread th1 { push };
        std::jthread th2 { push };
        std::jthread th3 { push };
        std::jthread th4 { push };
        std::this_thread::sleep_for(1ms);
        std::jthread th5 { pop };
    }

    EXPECT_EQ(400, popCount);
}
