#pragma once

#include <chrono>
#include <future>
#include <type_traits>

namespace
{
template <template <class> class FutureLike, class ValueType>
    requires std::is_same_v<FutureLike<ValueType>, std::future<ValueType>> ||
             std::is_same_v<FutureLike<ValueType>, std::shared_future<ValueType>>
class FutureBase
{
  public:
    [[nodiscard]] inline ValueType get()
    {
        if (!internal.valid())
        {
            throw std::future_error{std::future_errc::future_already_retrieved};
        }
        return internal.get();
    }

    /**
     * Returns a future status after specified timeout passed.
     *
     * @param timeout_s how long should poll wait before returning.
     * @return std::future_status::ready or std::future_status::timeout.
     */
    [[nodiscard]] inline std::future_status poll(uint64_t timeout_s = 0) const noexcept
    {
        return internal.wait_for(std::chrono::seconds(timeout_s));
    };

    [[nodiscard]] inline bool isReady() const noexcept
    {
        return poll() == std::future_status::ready;
    };

  protected:
    FutureLike<ValueType> internal;
    FutureBase(FutureLike<ValueType> f) : internal{std::move(f)} {}
};
} // namespace

namespace cfdp::runtime::future
{
template <class T>
class SharedFuture : public FutureBase<std::shared_future, T>
{
  public:
    explicit SharedFuture(std::shared_future<T> f) : FutureBase<std::shared_future, T>{std::move(f)}
    {}

    ~SharedFuture()                              = default;
    SharedFuture(const SharedFuture&)            = default;
    SharedFuture& operator=(SharedFuture const&) = default;
    SharedFuture(SharedFuture&&)                 = default;

    SharedFuture& operator=(SharedFuture&&) = delete;
};

template <class T>
class Future : public FutureBase<std::future, T>
{
  public:
    explicit Future(std::future<T> f) : FutureBase<std::future, T>{std::move(f)} {}

    ~Future()                   = default;
    Future(Future&&)            = default;
    Future& operator=(Future&&) = default;

    Future(const Future&)            = delete;
    Future& operator=(Future const&) = delete;

    [[nodiscard]] SharedFuture<T> makeShared()
    {
        if (!this->internal.valid())
        {
            throw std::future_error{std::future_errc::future_already_retrieved};
        }

        return SharedFuture<T>{this->internal.share()};
    };
};
} // namespace cfdp::runtime::future
