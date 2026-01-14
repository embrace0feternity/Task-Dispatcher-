#include <gtest/gtest.h>
#include <thread>

#include "queue/unbounded_queue.hpp"

using namespace dispatcher::queue;
using namespace dispatcher;

class UQTestGroup : public testing::Test {
protected:
    using Task = typename UnboundedQueue::Task;

    UnboundedQueue uq;
    
    int value1 = 0;
    int value2 = 0;

    Task t1 = [&](){
        value1 = 5;
    };
    Task t2 = [&](){
        value2 = 4 + value1;
    };
};


///
///
///

TEST_F(UQTestGroup, PushPopSingleThread) {
    EXPECT_FALSE(uq.try_pop().has_value());

    uq.push(t1);
    uq.push(t2);

    auto rt = uq.try_pop();
    EXPECT_TRUE(rt.has_value());
    rt.value()();
    EXPECT_EQ(5, value1);

    rt = uq.try_pop();
    EXPECT_TRUE(rt.has_value());
    rt.value()();
    EXPECT_EQ(9, value2);
}

///
///
///

TEST_F(UQTestGroup, PushMultiThread) {
    std::atomic<int> pushCount = 0;

    auto push = [&](){
        for (int i = 0; i < 5000; ++i) {
            uq.push(t1);
            pushCount.fetch_add(1, std::memory_order_relaxed);
        }
    };

    {
        std::jthread th1 { push };
        std::jthread th2 { push };
    }

    EXPECT_EQ(10000, pushCount);
}


///
///
///

TEST_F(UQTestGroup, PopMultiThread) {
    std::atomic<int> popCount = 0;

    auto pop = [&](){
        while (true) {
            auto rt = uq.try_pop();
            if (!rt.has_value()) {
                break;
            }
            popCount.fetch_add(1, std::memory_order_relaxed);
        }
    };

    for (int i = 0; i < 10000; ++i) {
        uq.push(t1);
    }

    {
        std::jthread th1 { pop };
        std::jthread th2 { pop };
    }

    EXPECT_EQ(10000, popCount);
}