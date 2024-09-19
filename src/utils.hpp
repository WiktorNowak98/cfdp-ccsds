#pragma once

#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>

namespace utils
{
std::vector<uint8_t> intToBigEndianBytes(uint64_t value, uint8_t size);
template <class T> inline void concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst);

template <class EnumType>
    requires std::is_enum_v<EnumType>
decltype(auto) toUnderlying(EnumType e)
{
    return static_cast<std::underlying_type_t<EnumType>>(e);
}
} // namespace utils

template <class T>
inline void utils::concatenateVectorsInplace(std::vector<T>& src, std::vector<T>& dst)
{
    dst.insert(dst.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
}
