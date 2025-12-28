#include <gtest/gtest.h>

#include "task_dispatcher.hpp"

using namespace dispatcher;

class TaskDispatcherTestGroup : public testing::Test {
protected:
    using Task = typename TaskDispatcher::Task;

    std::atomic<int> hpCounter = 0;
    int hpExpected = 500;
    Task hp = [&](){
        hpCounter.fetch_add(1);
    };

    std::atomic<int> npCounter = 0;
    int npExpected = 1500;
    /// Not used in multithreading test
    Task np = [&](){
        /// This condition will perfectly work for single thread test,
        ///  which shows right task priority execution.
        if (hpExpected == hpCounter) {
            npCounter.fetch_add(1);
        }
    };
};

///
///
///

TEST_F(TaskDispatcherTestGroup, scheduleSingleThread) { 
    std::size_t threadNumber = 1;
    {
        TaskDispatcher td { threadNumber };

        /// Add Normal priority tasks, then High ones.
        ///  They must execute in a way:
        ///  HP_1, HP_2, ... HP_n, NP_1, NP_2, ... NP_m
        for (int i = 0; i < hpExpected; ++i) {
            td.schedule(TaskPriority::High, hp);
        }
        for (int i = 0; i < npExpected; ++i) {
            td.schedule(TaskPriority::Normal, np);
        }
        /// Because there is main thread and 1 worker, the program
        ///  is still multithreading. Passing high priority tasks, 
        ///  after normal tasks might lead to another execution order.
    }

    EXPECT_EQ(hpExpected, hpCounter);
    EXPECT_EQ(npExpected, npCounter);
}

///
///
///

TEST_F(TaskDispatcherTestGroup, scheduleMultiThread) { 
    {
        TaskDispatcher td { /* threadNumber = hardware_concurrency */ };

        /// Add Normal priority tasks, then High ones
        for (int i = 0; i < npExpected; ++i) {
            td.schedule(TaskPriority::Normal, [&]{ npCounter++; });
        }
        for (int i = 0; i < hpExpected; ++i) {
            td.schedule(TaskPriority::High, hp);
        }
    }

    EXPECT_EQ(hpExpected, hpCounter);
    EXPECT_EQ(npExpected, npCounter);
}