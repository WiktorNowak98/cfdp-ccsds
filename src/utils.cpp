#include "utils.hpp"

#include <span>

std::vector<uint8_t> utils::intToBigEndianBytes(uint64_t value, uint8_t size)
{
    std::span<uint8_t> view{std::bit_cast<uint8_t*>(&value), size};

#if defined(__APPLE__) || defined(__linux__) || defined(_WIN32)
    std::vector<uint8_t> bytes{view.rbegin(), view.rend()};
#else
#error Big Endian systems are currently not supported.
#endif
    return bytes;
};
