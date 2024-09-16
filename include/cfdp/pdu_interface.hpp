#pragma once

#include <concepts>
#include <cstdint>
#include <vector>

namespace cfdp::pdu
{

constexpr uint8_t BITS_IN_BYTE = 8;

class PduInterface
{
  public:
    PduInterface()                               = default;
    virtual ~PduInterface()                      = default;

    PduInterface(const PduInterface&)            = delete;
    PduInterface& operator=(PduInterface const&) = delete;
    PduInterface(PduInterface&&)                 = delete;
    PduInterface& operator=(PduInterface&&)      = delete;

    virtual inline uint16_t getRawSize()         = 0;
    virtual std::vector<uint8_t> encodeToBytes() = 0;

  protected:
    template <class Number>
        requires std::unsigned_integral<Number>
    std::vector<uint8_t> intToBeBytes(Number value)
    {
        uint8_t amountOfBytes = sizeof(Number);
        auto bytes            = std::vector<uint8_t>{};

        bytes.reserve(amountOfBytes);

        for (uint8_t idx = 0; idx < amountOfBytes; idx++)
            bytes[idx] = (value >> (idx * BITS_IN_BYTE));

        return bytes;
    }
};
} // namespace cfdp::pdu
