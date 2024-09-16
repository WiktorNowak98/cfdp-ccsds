#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

template <class EnumType>
    requires std::is_enum_v<EnumType>
constexpr auto toUnderlying(EnumType e)
{
    return static_cast<std::underlying_type_t<EnumType>>(e);
}

template <class UnsignedInteger>
    requires std::unsigned_integral<UnsignedInteger>
std::vector<uint8_t> intToBytes(UnsignedInteger value);
