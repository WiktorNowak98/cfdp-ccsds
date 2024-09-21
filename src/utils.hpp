#pragma once

#include <cstdint>
#include <iterator>
#include <span>
#include <type_traits>
#include <vector>

#include "internal_exceptions.hpp"

namespace utils
{
std::vector<uint8_t> intToBigEndianBytes(uint64_t value, uint8_t size);

template <class T>
    requires std::unsigned_integral<T>
T bigEndianBytesToInt(std::span<uint8_t const> memoryView);

template <class T>
    requires std::unsigned_integral<T>
T bigEndianBytesToIntValidated(std::span<uint8_t const> memoryView, uint32_t offset, uint32_t size);

template <class T>
inline void concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst);

template <class T>
    requires std::is_enum_v<T>
inline decltype(auto) toUnderlying(T e) noexcept;
} // namespace utils

template <class T>
    requires std::unsigned_integral<T>
T utils::bigEndianBytesToInt(std::span<uint8_t const> memoryView)
{
    if (memoryView.size() > sizeof(T))
    {
        throw cfdp::exception::BytesDecodeException{
            "Passed span contains more bytes than type T can fit"};
    }

    T result{};
    for (const auto& it : memoryView)
    {
        result = (result << 8) + it;
    }

    return result;
}

template <class T>
    requires std::unsigned_integral<T>
T utils::bigEndianBytesToIntValidated(std::span<uint8_t const> memoryView, uint32_t offset,
                                      uint32_t size)
{
    // NOTE: 21.09.2024 <@uncommon-nickname>
    // Checking this size is rather important, creating a subspan
    // with either offset or size out of the original range is an
    // undefined behaviour!
    auto requiredSubspanSize = offset + size;

    if (memoryView.size() < requiredSubspanSize)
    {
        throw cfdp::exception::BytesDecodeException{
            "Span is too short to read expected amount of bytes"};
    }
    auto subspan = memoryView.subspan(offset, size);

    return utils::bigEndianBytesToInt<T>(subspan);
}

template <class T>
inline void utils::concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst)
{
    dst.insert(dst.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
}

template <class T>
    requires std::is_enum_v<T>
inline decltype(auto) utils::toUnderlying(T e) noexcept
{
    return static_cast<std::underlying_type_t<T>>(e);
}
