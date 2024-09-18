#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

namespace utils
{
std::vector<uint8_t> intToBytes(uint64_t value, uint8_t size);

template <class EnumType>
    requires std::is_enum_v<EnumType>
decltype(auto) toUnderlying(EnumType e)
{
    return static_cast<std::underlying_type_t<EnumType>>(e);
}
} // namespace utils
