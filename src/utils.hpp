#pragma once

#include <concepts>
#include <cstdint>
#include <exception>
#include <iterator>
#include <span>
#include <type_traits>
#include <vector>

namespace utils
{
std::vector<uint8_t> intToBigEndianBytes(uint64_t value, uint8_t size);

template <class T>
    requires std::unsigned_integral<T>
T bigEndianBytesToInt(const std::span<uint8_t> memoryView);

template <class T>
inline void concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst);

template <class T>
    requires std::is_enum_v<T>
inline decltype(auto) toUnderlying(T e);
} // namespace utils

template <class T>
    requires std::unsigned_integral<T>
T utils::bigEndianBytesToInt(const std::span<uint8_t> memoryView)
{
    if (memoryView.size_bytes() > sizeof(T))
    {
        throw std::invalid_argument{"Passed span contains more bytes than type T can fit"};
    }

    T result{};
    for (const auto& it : memoryView)
    {
        result = (result << 8) + it;
    }
    return result;
}

template <class T>
void utils::concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst)
{
    dst.insert(dst.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
}

template <class T>
    requires std::is_enum_v<T>
inline decltype(auto) utils::toUnderlying(T e)
{
    return static_cast<std::underlying_type_t<T>>(e);
}
