#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace cfdp::runtime::atomic
{
template <class T>
class AtomicQueue
{
  public:
    T pop() noexcept;

    void push(const T& item) noexcept;
    void push(T&& item) noexcept;

    [[nodiscard]] size_t sizeNow() const noexcept { return queue.size(); };

  private:
    std::queue<T> queue{};
    mutable std::mutex mutex{};
    mutable std::condition_variable condition{};

    void emplace(T&& item) noexcept;
};
} // namespace cfdp::runtime::atomic

template <class T>
void cfdp::runtime::atomic::AtomicQueue<T>::push(const T& item) noexcept
{
    emplace(item);
}

template <class T>
void cfdp::runtime::atomic::AtomicQueue<T>::push(T&& item) noexcept
{
    emplace(std::move(item));
}

template <class T>
T cfdp::runtime::atomic::AtomicQueue<T>::pop() noexcept
{
    std::unique_lock<std::mutex> lock{mutex};
    condition.wait(lock, [this]() { return !this->queue.empty(); });

    auto item = queue.front();
    queue.pop();

    return item;
}

template <class T>
void cfdp::runtime::atomic::AtomicQueue<T>::emplace(T&& item) noexcept
{
    {
        std::scoped_lock<std::mutex> lock{mutex};
        queue.push(std::move(item));
    }
    condition.notify_one();
}
