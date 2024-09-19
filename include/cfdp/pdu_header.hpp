#pragma once

#include <cstdint>
#include <vector>

#include "pdu_enums.hpp"
#include "pdu_interface.hpp"

namespace cfdp::pdu::header
{
/*
    Without the last 3 fields, the rest of the PDU header has constant
    size. Calculating all used bits in order:
    sizeBits = 3 + 1 + 1 + 1 + 1 + 1 + 16 + 1 + 3 + 1 + 3 = 32
*/
constexpr uint16_t CONST_HEADER_SIZE_BYTES = 32 / 8;

class PduHeader : PduInterface
{
  public:
    PduHeader(uint8_t version, PduType pduType, Direction direction,
              TransmissionMode transmissionMode, CrcFlag crcFlag, LargeFileFlag largeFileFlag,
              uint16_t pduDataFieldLength, SegmentationControl segmentationControl,
              uint8_t lengthOfEntityIDs, SegmentMetadataFlag segmentMetadataFlag,
              uint8_t lengthOfTransaction, uint64_t sourceEntityID,
              uint64_t transactionSequenceNumber, uint64_t destinationEntityID)
        : version(version), pduType(pduType), direction(direction),
          transmissionMode(transmissionMode), crcFlag(crcFlag), largeFileFlag(largeFileFlag),
          pduDataFieldLength(pduDataFieldLength), segmentationControl(segmentationControl),
          lengthOfEntityIDs(lengthOfEntityIDs), segmentMetadataFlag(segmentMetadataFlag),
          lengthOfTransaction(lengthOfTransaction), sourceEntityID(sourceEntityID),
          transactionSequenceNumber(transactionSequenceNumber),
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
    uint8_t lengthOfTransaction : 3;
    uint64_t sourceEntityID;
    uint64_t transactionSequenceNumber;
    uint64_t destinationEntityID;
};
} // namespace cfdp::pdu::header

inline uint16_t cfdp::pdu::header::PduHeader::getRawSize() const
{
    return CONST_HEADER_SIZE_BYTES + (2 * lengthOfEntityIDs) + lengthOfTransaction;
};
