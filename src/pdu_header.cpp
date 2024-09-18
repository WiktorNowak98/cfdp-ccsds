#include <cfdp/pdu_header.hpp>

#include "utils.hpp"

/*
    Without the last 3 fields, the rest of the PDU header has constant
    size. Calculating all used bits in order:
    sizeBits = 3 + 1 + 1 + 1 + 1 + 1 + 16 + 1 + 3 + 1 + 3 = 32
*/
constexpr uint16_t CONST_HEADER_SIZE_BYTES = 32 / 8;

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> && std::unsigned_integral<SequenceNumberType>
inline uint16_t cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::getRawSize() const
{
    return CONST_HEADER_SIZE_BYTES + (2 * sizeof(EntityIDType)) + sizeof(SequenceNumberType);
};

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> && std::unsigned_integral<SequenceNumberType>
std::vector<uint8_t> cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::encodeToBytes() const
{
    auto headerSize    = getRawSize();
    auto encodedHeader = std::vector<uint8_t>{};

    encodedHeader.reserve(headerSize);

    uint16_t realPduDataFieldLength = pduDataFieldLength + 4 * (crcFlag == CrcFlag::CrcPresent);

    auto pduDataFieldLengthBytes        = utils::intToBytes(realPduDataFieldLength);
    auto sourceEntityIDBytes            = utils::intToBytes(sourceEntityID);
    auto transactionSequenceNumberBytes = utils::intToBytes(transactionSequenceNumber);
    auto destinationEntityIDBytes       = utils::intToBytes(destinationEntityID);

    encodedHeader.push_back(buildFirstByte());

    encodedHeader.insert(encodedHeader.end(), pduDataFieldLengthBytes.begin(),
                         pduDataFieldLengthBytes.end());

    encodedHeader.push_back(buildFourthByte());

    encodedHeader.insert(encodedHeader.end(), sourceEntityIDBytes.begin(),
                         sourceEntityIDBytes.end());
    encodedHeader.insert(encodedHeader.end(), transactionSequenceNumberBytes.begin(),
                         transactionSequenceNumberBytes.end());
    encodedHeader.insert(encodedHeader.end(), destinationEntityIDBytes.begin(),
                         destinationEntityIDBytes.end());

    return encodedHeader;
};

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> && std::unsigned_integral<SequenceNumberType>
uint8_t cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::buildFirstByte() const
{
    return (version << 5) | (utils::toUnderlying(pduType) << 4) |
           (utils::toUnderlying(direction) << 3) | (utils::toUnderlying(transmissionMode) << 2) |
           (utils::toUnderlying(crcFlag) << 1) | (utils::toUnderlying(largeFileFlag) << 0);
}

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> && std::unsigned_integral<SequenceNumberType>
uint8_t cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::buildFourthByte() const
{
    return (utils::toUnderlying(segmentationControl) << 7) | ((lengthOfEntityIDs - 1) << 4) |
           (utils::toUnderlying(segmentMetadataFlag) << 3) |
           ((lengthOfTransactionSequenceNumber - 1) << 0);
}
