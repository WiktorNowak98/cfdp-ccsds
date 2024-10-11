#pragma once

#include <chrono>
#include <future>

namespace cfdp::runtime::future
{
template <class T>
class SharedFuture
{
  public:
    explicit SharedFuture(std::shared_future<T> f) : internal(f) {}

    ~SharedFuture()                              = default;
    SharedFuture(const SharedFuture&)            = default;
    SharedFuture& operator=(SharedFuture const&) = default;

    SharedFuture(SharedFuture&&)            = delete;
    SharedFuture& operator=(SharedFuture&&) = delete;

    [[nodiscard]] inline T get() { return internal.get(); }

    [[nodiscard]] inline std::future_status poll() const noexcept
    {
        return internal.wait_for(std::chrono::seconds(0));
    };

    [[nodiscard]] inline bool isReady() const noexcept
    {
        return poll() == std::future_status::ready;
    };

  private:
    std::shared_future<T> internal;
};

template <class T>
class Future
{
  public:
    explicit Future(std::future<T> f) : internal(std::move(f)) {}
    explicit Future(std::future<T>& f) : internal(std::move(f)) {}

    ~Future()                   = default;
    Future(Future&&)            = default;
    Future& operator=(Future&&) = default;

    Future(const Future&)            = delete;
    Future& operator=(Future const&) = delete;

    [[nodiscard]] inline T get()
    {
        // NOTE: 11.10.2024 <@uncommon-nickname>
        // This shields us from multiple gets and accessing a future
        // that was moved to be a shared future. Some implementations
        // of std::future check it by themselves, but on clang I segfault.
        if (!internal.valid())
        {
            throw std::future_error{std::future_errc::future_already_retrieved};
        }
        return internal.get();
    }

    [[nodiscard]] inline std::future_status poll() const noexcept
    {
        return internal.wait_for(std::chrono::seconds(0));
    };

    [[nodiscard]] inline bool isReady() const noexcept
    {
        return poll() == std::future_status::ready;
    };

    [[nodiscard]] SharedFuture<T> makeShared()
    {
        // NOTE: 11.10.2024 <@uncommon-nickname>
        // This shields us from trying to share a future that was
        // consumed or shared before. Some implementations of std::future
        // check it by themselves, but on clang I segfault.
        if (!internal.valid())
        {
            throw std::future_error{std::future_errc::future_already_retrieved};
        }

        return SharedFuture<T>{internal.share()};
    };

  private:
    std::future<T> internal;
};
} // namespace cfdp::runtime::future
