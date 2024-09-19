#include <cfdp/pdu_header.hpp>

#include "utils.hpp"

std::vector<uint8_t> cfdp::pdu::header::PduHeader::encodeToBytes() const
{
    auto headerSize    = getRawSize();
    auto encodedHeader = std::vector<uint8_t>{};

    encodedHeader.reserve(headerSize);

    uint16_t realPduDataFieldLength = pduDataFieldLength + 4 * (crcFlag == CrcFlag::CrcPresent);

    uint8_t firstByte =
        (version << 5) | (utils::toUnderlying(pduType) << 4) |
        (utils::toUnderlying(direction) << 3) | (utils::toUnderlying(transmissionMode) << 2) |
        (utils::toUnderlying(crcFlag) << 1) | (utils::toUnderlying(largeFileFlag) << 0);

    encodedHeader.push_back(firstByte);

    auto pduDataFieldLengthBytes =
        utils::intToBigEndianBytes(realPduDataFieldLength, sizeof(uint16_t));

    utils::concatenateVectorsInplace(pduDataFieldLengthBytes, encodedHeader);

    uint8_t fourthByte =
        (utils::toUnderlying(segmentationControl) << 7) | ((lengthOfEntityIDs - 1) << 4) |
        (utils::toUnderlying(segmentMetadataFlag) << 3) | ((lengthOfTransaction - 1) << 0);

    encodedHeader.push_back(fourthByte);

    auto sourceEntityBytes = utils::intToBigEndianBytes(sourceEntityID, lengthOfEntityIDs);

    utils::concatenateVectorsInplace(sourceEntityBytes, encodedHeader);

    auto transactionBytes =
        utils::intToBigEndianBytes(transactionSequenceNumber, lengthOfTransaction);

    utils::concatenateVectorsInplace(transactionBytes, encodedHeader);

    auto destinationEntityBytes =
        utils::intToBigEndianBytes(destinationEntityID, lengthOfEntityIDs);

    utils::concatenateVectorsInplace(destinationEntityBytes, encodedHeader);

    return encodedHeader;
};
