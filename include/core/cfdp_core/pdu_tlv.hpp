#pragma once

#include "cfdp_core/pdu_interface.hpp"
#include <span>

namespace cfdp::pdu::tlv
{
class EntityId : PduInterface
{
  public:
    EntityId(uint8_t lengthOfEntityID, uint64_t faultEntityID)
        : lengthOfEntityID(lengthOfEntityID), faultEntityID(faultEntityID){};
    EntityId(std::span<uint8_t const> memory);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

    [[nodiscard]] inline uint16_t getRawSize() const override
    {
        return sizeof(uint8_t) + sizeof(uint8_t) + lengthOfEntityID;
    };

    uint8_t lengthOfEntityID;
    uint64_t faultEntityID;
};
} // namespace cfdp::pdu::tlv
