#include "utils.hpp"

std::vector<uint8_t> cfdp::internal::utils::intToBytes(uint64_t value, uint8_t size)
{
    if (size > sizeof(uint64_t))
    {
        throw exception::EncodeToBytesException{"Size can't be larger than 8 bytes"};
    }

    std::span<uint8_t> view{std::bit_cast<uint8_t*>(&value), size};

    return std::vector<uint8_t>{view.rbegin(), view.rend()};
};
