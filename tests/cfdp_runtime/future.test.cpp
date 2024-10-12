#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_runtime/future.hpp>

#include <future>

using ::cfdp::runtime::future::Future;

class TestFuture : public ::testing::Test
{
  protected:
    std::future<int> testFuture;
    std::promise<int> testPromise;

    TestFuture()
    {
        testPromise = std::promise<int>{};
        testFuture  = testPromise.get_future();
    }
};

TEST_F(TestFuture, FutureConsumesStdFutureObject)
{
    auto future = Future{std::move(testFuture)};

    ASSERT_FALSE(testFuture.valid());
}

TEST_F(TestFuture, FutureCantBeGetTwoTimes)
{
    testPromise.set_value(1);

    auto future = Future{std::move(testFuture)};
    auto _      = future.get();

    EXPECT_THROW(auto _ = future.get(), std::future_error);
}

TEST_F(TestFuture, FutureCanBeGet)
{
    testPromise.set_value(1);
    auto future = Future{std::move(testFuture)};

    ASSERT_EQ(future.get(), 1);
}

TEST_F(TestFuture, NotReadyFutureHasStatusTimeout)
{
    auto future = Future{std::move(testFuture)};

    ASSERT_EQ(future.poll(), std::future_status::timeout);
}

TEST_F(TestFuture, ReadyFutureHasStatusReady)
{
    testPromise.set_value(1);
    auto future = Future{std::move(testFuture)};

    ASSERT_EQ(future.poll(), std::future_status::ready);
}

TEST_F(TestFuture, FutureIsNotReady)
{
    auto future = Future{std::move(testFuture)};

    ASSERT_FALSE(future.isReady());
}

TEST_F(TestFuture, FutureIsReady)
{
    testPromise.set_value(1);
    auto future = Future{std::move(testFuture)};

    ASSERT_TRUE(future.isReady());
}

TEST_F(TestFuture, FutureConsumedWhenShared)
{
    auto future       = Future{std::move(testFuture)};
    auto sharedFuture = future.makeShared();

    EXPECT_THROW(auto _ = future.get(), std::future_error);
}

TEST_F(TestFuture, FutureCannotBeSharedTwoTimes)
{
    auto future       = Future{std::move(testFuture)};
    auto sharedFuture = future.makeShared();

    EXPECT_THROW(auto _ = future.makeShared(), std::future_error);
}

TEST_F(TestFuture, FutureCannotBeSharedAfterConsumed)
{
    testPromise.set_value(1);

    auto future = Future{std::move(testFuture)};
    auto _      = future.get();

    EXPECT_THROW(auto _ = future.makeShared(), std::future_error);
}

TEST_F(TestFuture, SharedFutureCanBeGetMultipleTimes)
{
    testPromise.set_value(1);

    auto sharedFuture = Future{std::move(testFuture)}.makeShared();

    ASSERT_EQ(sharedFuture.get(), 1);
    ASSERT_EQ(sharedFuture.get(), 1);
}

TEST_F(TestFuture, SharedFutureCanBeCopied)
{
    testPromise.set_value(1);

    auto sharedFuture = Future{std::move(testFuture)}.makeShared();
    auto secondFuture = sharedFuture;

    ASSERT_EQ(sharedFuture.get(), 1);
    ASSERT_EQ(secondFuture.get(), 1);
}
