#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "pdu_enums.hpp"
#include "pdu_interface.hpp"

namespace cfdp::pdu::header
{
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
    PduHeader(std::span<uint8_t const> memory);

    [[nodiscard]] inline uint16_t getRawSize() const override
    {
        return const_header_size_bytes + (2 * lengthOfEntityIDs) + lengthOfTransaction;
    };

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

  private:
    // Used version of the CFDP protocol. Between 0 and 7.
    uint8_t version;
    PduType pduType;
    Direction direction;
    TransmissionMode transmissionMode;
    CrcFlag crcFlag;
    LargeFileFlag largeFileFlag;
    uint16_t pduDataFieldLength;
    SegmentationControl segmentationControl;
    // Number of bytes in `entityID`. Between 1 and 8.
    uint8_t lengthOfEntityIDs;
    SegmentMetadataFlag segmentMetadataFlag;
    // Number of bytes in `transactionNumber`. Between 1 and 8.
    uint8_t lengthOfTransaction;
    uint64_t sourceEntityID;
    uint64_t transactionSequenceNumber;
    uint64_t destinationEntityID;

    // Without last three fields, PDU header has constant size of 32 bits.
    static constexpr uint16_t const_header_size_bytes = sizeof(uint32_t);
    // Last three fields have to AT LEAST contain a single byte each.
    static constexpr uint16_t min_header_size_bytes = const_header_size_bytes + 3;
};
} // namespace cfdp::pdu::header
