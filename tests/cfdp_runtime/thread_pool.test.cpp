#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_runtime/thread_pool.hpp>

#include <chrono>
#include <future>
#include <thread>

using ::cfdp::runtime::future::Future;
using ::cfdp::runtime::thread_pool::ThreadPool;

class ThreadPoolTest : public ::testing::Test
{
  protected:
    ThreadPool pool;

    ThreadPoolTest() : pool(1) {};
    ~ThreadPoolTest() { pool.shutdown(); };
};

TEST_F(ThreadPoolTest, TestNoTasksAreDroppedFromSchedule)
{
    auto func = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return 1;
    };

    // Dispatch more tasks that the pool can handle.
    auto futures = std::vector<Future<int>>{};
    futures.reserve(50);

    for (auto i = 0; i < 50; ++i)
    {
        futures.push_back(pool.dispatchTask(func));
    }

    for (auto& future : futures)
    {
        ASSERT_EQ(future.get(), 1);
    }
}

TEST_F(ThreadPoolTest, TestQueuedTaskIsDispatched)
{
    auto func = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 1;
    };

    auto instantFuture = pool.dispatchTask(func);
    auto waitingFuture = pool.dispatchTask(func);

    // Not ready (in progress).
    ASSERT_EQ(instantFuture.poll(), std::future_status::timeout);
    // Not ready (queued).
    ASSERT_EQ(waitingFuture.poll(), std::future_status::timeout);
    // Make sure the first future has time to finish.
    std::this_thread::sleep_for(std::chrono::milliseconds(12));

    // Done.
    ASSERT_EQ(instantFuture.poll(), std::future_status::ready);
    // Not ready (in progress).
    ASSERT_EQ(waitingFuture.poll(), std::future_status::timeout);
    // Make sure the second future has time to finish.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Done.
    ASSERT_EQ(waitingFuture.poll(), std::future_status::ready);
}

TEST_F(ThreadPoolTest, NoMoreTasksScheduledAfterShutdown)
{
    auto func = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 1;
    };

    auto validFuture = pool.dispatchTask(func);
    // Wait to ensure the task is dispatched.
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    // Kill the pool, joining the running tasks.
    pool.shutdown();
    // Dispatch task which will not be scheduled.
    auto invalidFuture = pool.dispatchTask(func);
    // Wait for both futures to be surely finished if dispatched.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    ASSERT_TRUE(validFuture.isReady());
    ASSERT_FALSE(invalidFuture.isReady());
}
