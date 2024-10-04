#include <cfdp_core/utils.hpp>

#include <bit>

std::vector<uint8_t> cfdp::utils::intToBytes(uint64_t value, uint8_t size)
{
    if (size > sizeof(uint64_t))
    {
        throw exception::EncodeToBytesException{"Size can't be larger than 8 bytes"};
    }

    std::span<uint8_t> view{std::bit_cast<uint8_t*>(&value), size};

    return std::vector<uint8_t>{view.rbegin(), view.rend()};
};

size_t cfdp::utils::bytesNeeded(uint64_t number)
{
    size_t bitsNeeded  = std::bit_width(number);
    size_t bytesNeeded = bitsNeeded == 0 ? 1 : (bitsNeeded + 7) / 8;

    return bytesNeeded;
}
