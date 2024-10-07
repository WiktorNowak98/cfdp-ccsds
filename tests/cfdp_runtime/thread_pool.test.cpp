#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_runtime/thread_pool.hpp>

#include <chrono>
#include <thread>

using ::cfdp::runtime::thread_pool::ThreadPool;

TEST(A, B)
{
    auto pool = ThreadPool(5);

    std::function<int()> func = []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 1;
    };
    pool.dispatchTask(func);
    pool.shutdown();
}
