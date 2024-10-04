#pragma once

#include <cstdint>

namespace cfdp::pdu::header
{
enum class PduType : uint8_t
{
    FileDirective = 0b0,
    FileData      = 0b1,
};

enum class Direction : uint8_t
{
    TowardsReceiver = 0b0,
    TowardsSender   = 0b1,
};

enum class TransmissionMode : uint8_t
{
    Acknowledged   = 0b0,
    Unacknowledged = 0b1,
};

enum class CrcFlag : uint8_t
{
    CrcNotPresent = 0b0,
    CrcPresent    = 0b1,
};

enum class LargeFileFlag : uint8_t
{
    SmallFile = 0b0,
    LargeFile = 0b1,
};

enum class SegmentationControl : uint8_t
{
    BoundariesNotPreserved = 0b0,
    BoundariesPreserved    = 0b1,
};

enum class SegmentMetadataFlag : uint8_t
{
    NotPresent = 0b0,
    Present    = 0b1,
};
} // namespace cfdp::pdu::header
