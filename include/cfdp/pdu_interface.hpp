#pragma once

#include <concepts>
#include <cstdint>
#include <vector>

namespace cfdp::pdu
{
class PduInterface
{
  public:
    PduInterface()          = default;
    virtual ~PduInterface() = default;

    PduInterface(const PduInterface&)            = delete;
    PduInterface& operator=(PduInterface const&) = delete;
    PduInterface(PduInterface&&)                 = delete;
    PduInterface& operator=(PduInterface&&)      = delete;

    [[nodiscard]] virtual inline uint16_t getRawSize() const         = 0;
    [[nodiscard]] virtual std::vector<uint8_t> encodeToBytes() const = 0;
};
} // namespace cfdp::pdu
