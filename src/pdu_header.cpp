#include <cfdp/pdu_header.hpp>

#include "utils.hpp"

std::vector<uint8_t> cfdp::pdu::header::PduHeader::encodeToBytes() const
{
    auto headerSize    = getRawSize();
    auto encodedHeader = std::vector<uint8_t>(headerSize);

    uint16_t realPduDataFieldLength = pduDataFieldLength + 4 * (crcFlag == CrcFlag::CrcPresent);

    uint8_t firstByte =
        (version << 5) | (utils::toUnderlying(pduType) << 4) |
        (utils::toUnderlying(direction) << 3) | (utils::toUnderlying(transmissionMode) << 2) |
        (utils::toUnderlying(crcFlag) << 1) | (utils::toUnderlying(largeFileFlag) << 0);

    encodedHeader.push_back(firstByte);

    auto pduDataFieldLengthBytes = utils::intToBytes(realPduDataFieldLength, sizeof(uint16_t));

    encodedHeader.insert(encodedHeader.end(), pduDataFieldLengthBytes.begin(),
                         pduDataFieldLengthBytes.end());

    uint8_t fourthByte =
        (utils::toUnderlying(segmentationControl) << 7) | ((lengthOfEntityIDs - 1) << 4) |
        (utils::toUnderlying(segmentMetadataFlag) << 3) | ((lengthOfTransaction - 1) << 0);

    encodedHeader.push_back(fourthByte);

    auto sourceEntityIDBytes = utils::intToBytes(sourceEntityID, lengthOfEntityIDs);
    auto transactionSequenceNumberBytes =
        utils::intToBytes(transactionSequenceNumber, lengthOfTransaction);
    auto destinationEntityIDBytes = utils::intToBytes(destinationEntityID, lengthOfEntityIDs);

    encodedHeader.insert(encodedHeader.end(), sourceEntityIDBytes.begin(),
                         sourceEntityIDBytes.end());
    encodedHeader.insert(encodedHeader.end(), transactionSequenceNumberBytes.begin(),
                         transactionSequenceNumberBytes.end());
    encodedHeader.insert(encodedHeader.end(), destinationEntityIDBytes.begin(),
                         destinationEntityIDBytes.end());

    return encodedHeader;
};
