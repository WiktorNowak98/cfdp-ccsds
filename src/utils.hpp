#pragma once

#include <cstdint>
#include <iterator>
#include <span>
#include <type_traits>
#include <vector>

#include "internal_exceptions.hpp"

namespace utils
{
std::vector<uint8_t> intToBytes(uint64_t value, uint8_t size);

template <class T>
    requires std::unsigned_integral<T>
T bytesToInt(std::span<uint8_t const> memory, uint32_t offset, uint32_t size);

template <class T>
inline void concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst)
{
    dst.insert(dst.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
}

template <class T>
    requires std::is_enum_v<T>
inline decltype(auto) toUnderlying(T e) noexcept
{
    return static_cast<std::underlying_type_t<T>>(e);
}
} // namespace utils

template <class T>
    requires std::unsigned_integral<T>
T utils::bytesToInt(std::span<uint8_t const> memory, uint32_t offset, uint32_t size)
{
    // NOTE: 21.09.2024 <@uncommon-nickname>
    // Checking this size is rather important, creating a subspan
    // with either offset or size out of the original range causes an
    // undefined behaviour!
    if (memory.size() < offset + size)
    {
        throw cfdp::exception::BytesDecodeException{"Passed memory does not contain enough bytes"};
    }

    auto subspan = memory.subspan(offset, size);

    if (subspan.size() > sizeof(T))
    {
        throw cfdp::exception::BytesDecodeException{"Memory chunk will not fit in size T"};
    }

    T result{};

    for (const auto byte : subspan)
    {
        result = (result << 8) + byte;
    }

    return result;
}
