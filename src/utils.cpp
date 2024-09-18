#include "utils.hpp"

#include <span>

std::vector<uint8_t> utils::intToBytes(uint64_t value, uint8_t size)
{
    std::span<uint8_t> view{std::bit_cast<uint8_t*>(&value), size};
    return std::vector<uint8_t>{view.begin(), view.end()};
};
