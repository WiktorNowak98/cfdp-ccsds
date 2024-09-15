#pragma once

#include <cstdint>
#include <vector>

namespace cfdp::pdu
{
class PduInterface
{
  public:
    PduInterface()                               = default;
    virtual ~PduInterface()                      = default;

    PduInterface(const PduInterface&)            = delete;
    PduInterface& operator=(PduInterface const&) = delete;
    PduInterface(PduInterface&&)                 = delete;
    PduInterface& operator=(PduInterface&&)      = delete;

    virtual size_t getRawSize()                  = 0;
    virtual std::vector<uint8_t> encodeToBytes() = 0;
};
} // namespace cfdp::pdu
