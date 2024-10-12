#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_runtime/atomic_queue.hpp>

#include <array>
#include <numeric>
#include <ranges>
#include <thread>
#include <vector>

using ::cfdp::runtime::atomic::AtomicQueue;
using ::testing::UnorderedElementsAreArray;

class AtomicQueueTest : public ::testing::Test
{
  protected:
    template <class T>
    std::vector<T> drainQueue(AtomicQueue<T>& queue, size_t expected_size)
    {
        auto vec = std::vector<T>(expected_size);

        for (auto& item : vec)
        {
            item = queue.pop();
        }

        return vec;
    }
};

TEST_F(AtomicQueueTest, NoItemDroppedWhenMultipleProducers)
{
    auto queue = AtomicQueue<uint8_t>{};

    auto pusher = [&](size_t lb, size_t ub) {
        for (const auto& i : std::views::iota(lb, ub))
            queue.push(i);
    };
    auto workers = std::array<std::thread, 3>{
        std::thread{pusher,  0,  5},
        std::thread{pusher,  5, 10},
        std::thread{pusher, 10, 15},
    };

    for (auto& thread : workers)
        thread.join();

    ASSERT_EQ(queue.sizeNow(), 15);

    auto items = drainQueue(queue, 15);

    auto expected = std::vector<uint8_t>(15);
    std::iota(expected.begin(), expected.end(), 0);

    EXPECT_THAT(items, UnorderedElementsAreArray(expected));
}

TEST_F(AtomicQueueTest, MultipleProducersSingleConsumer)
{
    auto queue = AtomicQueue<uint8_t>{};

    auto pusher = [&](size_t lb, size_t ub) {
        for (const auto& i : std::views::iota(lb, ub))
            queue.push(i);
    };
    auto workers = std::array<std::thread, 2>{
        std::thread{pusher,  0, 10},
        std::thread{pusher, 10, 20},
    };
    auto items = drainQueue(queue, 20);

    for (auto& thread : workers)
        thread.join();

    ASSERT_EQ(queue.sizeNow(), 0);

    auto expected = std::vector<uint8_t>(20);
    std::iota(expected.begin(), expected.end(), 0);

    EXPECT_THAT(items, UnorderedElementsAreArray(expected));
}
