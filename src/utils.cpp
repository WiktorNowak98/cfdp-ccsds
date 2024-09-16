#include "utils.hpp"

inline constexpr uint8_t BITS_IN_BYTE = 8;

template <class UnsignedInteger>
    requires std::unsigned_integral<UnsignedInteger>
std::vector<uint8_t> intToBytes(UnsignedInteger value)
{
    // This cast is safe, the biggest we can get is 8.
    auto amountOfBytes = static_cast<uint8_t>(sizeof(UnsignedInteger));

    auto bytes = std::vector<uint8_t>{};
    bytes.reserve(amountOfBytes);

    for (uint8_t byteNum = 0; byteNum < amountOfBytes; byteNum++)
    {
        bytes[byteNum] = (value >> (byteNum * BITS_IN_BYTE));
    }
    return bytes;
};
