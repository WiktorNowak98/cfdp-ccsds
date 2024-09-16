#pragma once

#include <concepts>
#include <cstdint>
#include <vector>

#include "pdu_interface.hpp"

namespace cfdp::pdu
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

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> && std::unsigned_integral<SequenceNumberType>
class PduHeader : PduInterface
{
  public:
    PduHeader(uint8_t version, PduType pduType, Direction direction,
              TransmissionMode transmissionMode, CrcFlag crcFlag, LargeFileFlag largeFileFlag,
              uint16_t pduDataFieldLength, SegmentationControl segmentationControl,
              uint8_t lengthOfEntityIDs, SegmentMetadataFlag segmentMetadataFlag,
              uint8_t lengthOfTransactionSequenceNumber, EntityIDType sourceEntityID,
              SequenceNumberType transactionSequenceNumber, EntityIDType destinationEntityID)
        : version(version), pduType(pduType), direction(direction),
          transmissionMode(transmissionMode), crcFlag(crcFlag), largeFileFlag(largeFileFlag),
          pduDataFieldLength(pduDataFieldLength), segmentationControl(segmentationControl),
          lengthOfEntityIDs(lengthOfEntityIDs), segmentMetadataFlag(segmentMetadataFlag),
          lengthOfTransactionSequenceNumber(lengthOfTransactionSequenceNumber),
          sourceEntityID(sourceEntityID), transactionSequenceNumber(transactionSequenceNumber),
          destinationEntityID(destinationEntityID)
    {}

    [[nodiscard]] inline uint16_t getRawSize() const override;
    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

  private:
    uint8_t version : 3;
    PduType pduType;
    Direction direction;
    TransmissionMode transmissionMode;
    CrcFlag crcFlag;
    LargeFileFlag largeFileFlag;
    uint16_t pduDataFieldLength;
    SegmentationControl segmentationControl;
    // Number of bytes in `EntityIDType`. Between one and eight.
    uint8_t lengthOfEntityIDs : 3;
    SegmentMetadataFlag segmentMetadataFlag;
    // Number of bytes in `SequenceNumberType`. Between one and eight.
    uint8_t lengthOfTransactionSequenceNumber : 3;
    // Size defined by `lengthOfEntityIDs`.
    EntityIDType sourceEntityID;
    // Size defined by `lengthOfTransactionSequenceNumber`.
    SequenceNumberType transactionSequenceNumber;
    // Size defined by `lengthOfEntityIDs`.
    EntityIDType destinationEntityID;
};
} // namespace cfdp::pdu
