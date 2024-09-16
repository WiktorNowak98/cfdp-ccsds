#include <cfdp/pdu_header.hpp>

#include "utils.hpp"

/*
    Without the last 3 fields, the rest of the PDU header has constant
    size. Calculating all used bits in order:
    sizeBits = 3 + 1 + 1 + 1 + 1 + 1 + 16 + 1 + 3 + 1 + 3 = 32
*/
inline constexpr uint16_t CONST_HEADER_SIZE_BYTES      = 32 / 8;
inline constexpr uint8_t VERSION_POSITION              = 5;
inline constexpr uint8_t SEGMENTATION_CONTROL_POSITION = 7;

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
    encodedHeader.push_back((version << VERSION_POSITION) | (toUnderlying(pduType) << 4) |
                            (toUnderlying(direction) << 3) | (toUnderlying(transmissionMode) << 2) |
                            (toUnderlying(crcFlag) << 1) | (toUnderlying(largeFileFlag) << 0));

    auto realPduDataFieldLength =
        crcFlag == CrcFlag::CrcPresent ? pduDataFieldLength + 2 : pduDataFieldLength;

    auto pduDataFieldLengthBytes = intToBytes<uint16_t>(realPduDataFieldLength);

    encodedHeader.insert(encodedHeader.end(), pduDataFieldLengthBytes.begin(),
                         pduDataFieldLengthBytes.end());
    encodedHeader.push_back((toUnderlying(segmentationControl) << SEGMENTATION_CONTROL_POSITION) |
                            ((lengthOfEntityIDs - 1) << 4) |
                            (toUnderlying(segmentMetadataFlag) << 3) |
                            ((lengthOfTransactionSequenceNumber - 1) << 0));

    auto sourceEntityIDBytes = intToBytes<EntityIDType>(sourceEntityID);

    encodedHeader.insert(encodedHeader.end(), sourceEntityIDBytes.begin(),
                         sourceEntityIDBytes.end());

    auto transactionSequenceNumberBytes =
        intToBeBytes<SequenceNumberType>(transactionSequenceNumber);

    encodedHeader.insert(encodedHeader.end(), transactionSequenceNumberBytes.begin(),
                         transactionSequenceNumberBytes.end());

    auto destinationEntityIDBytes = intToBytes<EntityIDType>(destinationEntityID);

    encodedHeader.insert(encodedHeader.end(), destinationEntityIDBytes.begin(),
                         destinationEntityIDBytes.end());

    return encodedHeader;
};
