#include <cfdp/pdu_enums.hpp>
#include <cfdp/pdu_exceptions.hpp>
#include <cfdp/pdu_header.hpp>

#include "utils.hpp"

namespace
{
// First header byte related bitmasks.
constexpr uint8_t version_bitmask           = 0b1110'0000;
constexpr uint8_t pdu_type_bitmask          = 0b0001'0000;
constexpr uint8_t direction_bitmask         = 0b0000'1000;
constexpr uint8_t transmission_mode_bitmask = 0b0000'0100;
constexpr uint8_t crc_flag_bitmask          = 0b0000'0010;
constexpr uint8_t large_file_flag_bitmask   = 0b0000'0001;

// Fourth header byte related bitmasks.
constexpr uint8_t segmentation_control_bitmask  = 0b1000'0000;
constexpr uint8_t entity_id_length_bitmask      = 0b0111'0000;
constexpr uint8_t segment_metadata_flag_bitmask = 0b0000'1000;
constexpr uint8_t transaction_length_bitmask    = 0b0000'0111;
} // namespace

namespace utils     = ::cfdp::internal::utils;
namespace exception = ::cfdp::pdu::exception;

cfdp::pdu::header::PduHeader::PduHeader(
    uint8_t version, PduType pduType, Direction direction, TransmissionMode transmissionMode,
    CrcFlag crcFlag, LargeFileFlag largeFileFlag, uint16_t pduDataFieldLength,
    SegmentationControl segmentationControl, uint8_t lengthOfEntityIDs,
    SegmentMetadataFlag segmentMetadataFlag, uint8_t lengthOfTransaction, uint64_t sourceEntityID,
    uint64_t transactionSequenceNumber, uint64_t destinationEntityID)
    : version(version), pduType(pduType), direction(direction), transmissionMode(transmissionMode),
      crcFlag(crcFlag), largeFileFlag(largeFileFlag), pduDataFieldLength(pduDataFieldLength),
      segmentationControl(segmentationControl), lengthOfEntityIDs(lengthOfEntityIDs),
      segmentMetadataFlag(segmentMetadataFlag), lengthOfTransaction(lengthOfTransaction),
      sourceEntityID(sourceEntityID), transactionSequenceNumber(transactionSequenceNumber),
      destinationEntityID(destinationEntityID)
{
    if (lengthOfEntityIDs == 0 || lengthOfTransaction == 0)
    {
        throw exception::PduConstructionException{
            "Size of the entityIDs and transaction has to be > 0"};
    }

    if (lengthOfEntityIDs < utils::bytesNeeded(sourceEntityID) ||
        lengthOfEntityIDs < utils::bytesNeeded(destinationEntityID))
    {
        throw exception::PduConstructionException{
            "Entity ID is too large to fit in specified size"};
    }

    if (lengthOfTransaction < utils::bytesNeeded(transactionSequenceNumber))
    {
        throw exception::PduConstructionException{
            "Transaction number is too large to fit in specified size"};
    }

    if (sourceEntityID == destinationEntityID)
    {
        throw exception::PduConstructionException{
            "Source and destination entity IDs shouldn't be the same"};
    }
}

cfdp::pdu::header::PduHeader::PduHeader(std::span<uint8_t const> memory)
{
    if (memory.size() < min_header_size_bytes)
    {
        throw exception::DecodeFromBytesException{"Passed memory does not contain enough bytes"};
    }

    const auto firstByte = memory[0];

    version          = (firstByte & version_bitmask) >> 5;
    pduType          = PduType((firstByte & pdu_type_bitmask) >> 4);
    direction        = Direction((firstByte & direction_bitmask) >> 3);
    transmissionMode = TransmissionMode((firstByte & transmission_mode_bitmask) >> 2);
    crcFlag          = CrcFlag((firstByte & crc_flag_bitmask) >> 1);
    largeFileFlag    = LargeFileFlag((firstByte & large_file_flag_bitmask) >> 0);

    auto rawPduDataFieldLength = utils::bytesToInt<uint16_t>(memory, 1, 2);

    pduDataFieldLength =
        rawPduDataFieldLength - 4 * (static_cast<uint8_t>(crcFlag == CrcFlag::CrcPresent));

    const auto fourthByte = memory[3];

    segmentationControl = SegmentationControl((fourthByte & segmentation_control_bitmask) >> 7);
    segmentMetadataFlag = SegmentMetadataFlag((fourthByte & segment_metadata_flag_bitmask) >> 3);

    // To fit in 3 bits, CFDP standard specifies that the size is
    // encoded as a size - 1.
    lengthOfEntityIDs   = ((fourthByte & entity_id_length_bitmask) >> 4) + 1;
    lengthOfTransaction = ((fourthByte & transaction_length_bitmask) >> 0) + 1;

    sourceEntityID = utils::bytesToInt<uint64_t>(memory, 4, lengthOfEntityIDs);
    transactionSequenceNumber =
        utils::bytesToInt<uint64_t>(memory, 4 + lengthOfEntityIDs, lengthOfTransaction);
    destinationEntityID = utils::bytesToInt<uint64_t>(
        memory, 4 + lengthOfEntityIDs + lengthOfTransaction, lengthOfEntityIDs);
};

std::vector<uint8_t> cfdp::pdu::header::PduHeader::encodeToBytes() const
{
    const auto headerSize = getRawSize();
    auto encodedHeader    = std::vector<uint8_t>{};

    encodedHeader.reserve(headerSize);

    const uint16_t realPduDataFieldLength =
        pduDataFieldLength + 4 * (static_cast<uint8_t>(crcFlag == CrcFlag::CrcPresent));

    const uint8_t firstByte =
        (version << 5) | (utils::toUnderlying(pduType) << 4) |
        (utils::toUnderlying(direction) << 3) | (utils::toUnderlying(transmissionMode) << 2) |
        (utils::toUnderlying(crcFlag) << 1) | (utils::toUnderlying(largeFileFlag) << 0);

    encodedHeader.push_back(firstByte);

    auto pduDataFieldLengthBytes = utils::intToBytes(realPduDataFieldLength, sizeof(uint16_t));
    utils::concatenateVectorsInplace(pduDataFieldLengthBytes, encodedHeader);

    // To fit in 3 bits, CFDP standard specifies that the size is
    // encoded as a size - 1.
    const uint8_t fourthByte =
        (utils::toUnderlying(segmentationControl) << 7) | ((lengthOfEntityIDs - 1) << 4) |
        (utils::toUnderlying(segmentMetadataFlag) << 3) | ((lengthOfTransaction - 1) << 0);

    encodedHeader.push_back(fourthByte);

    auto sourceEntityBytes = utils::intToBytes(sourceEntityID, lengthOfEntityIDs);
    utils::concatenateVectorsInplace(sourceEntityBytes, encodedHeader);

    auto transactionBytes = utils::intToBytes(transactionSequenceNumber, lengthOfTransaction);
    utils::concatenateVectorsInplace(transactionBytes, encodedHeader);

    auto destinationEntityBytes = utils::intToBytes(destinationEntityID, lengthOfEntityIDs);
    utils::concatenateVectorsInplace(destinationEntityBytes, encodedHeader);

    return encodedHeader;
};
