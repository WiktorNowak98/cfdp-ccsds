#include "utils.hpp"

std::vector<uint8_t> utils::intToBigEndianBytes(uint64_t value, uint8_t size)
{
    if (size > sizeof(uint64_t))
    {
        throw std::invalid_argument{"Size can't be larger than 8."};
    }
    std::span<uint8_t> view{std::bit_cast<uint8_t*>(&value), size};

    return std::vector<uint8_t>{view.rbegin(), view.rend()};
};
