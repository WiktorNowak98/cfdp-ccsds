#include <cfdp/pdu_header.hpp>

constexpr uint8_t VERSION_POSITION              = 5;
constexpr uint8_t SEGMENTATION_CONTROL_POSITION = 7;

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> || std::unsigned_integral<SequenceNumberType>
inline uint16_t cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::getRawSize()
{
    return CONST_HEADER_SIZE_BYTES + (2 * sizeof(EntityIDType)) + sizeof(SequenceNumberType);
};

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> || std::unsigned_integral<SequenceNumberType>
std::vector<uint8_t> cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::encodeToBytes()
{
    auto headerSize    = getRawSize();
    auto encodedHeader = std::vector<uint8_t>{};

    encodedHeader.reserve(headerSize);
    encodedHeader.push_back((version << VERSION_POSITION) | ((uint8_t)pduType << 4) |
                            ((uint8_t)direction << 3) | ((uint8_t)transmissionMode << 2) |
                            ((uint8_t)crcFlag << 1) | ((uint8_t)largeFileFlag << 0));

    auto realPduDataFieldLength =
        crcFlag == CrcFlag::CrcPresent ? pduDataFieldLength + 2 : pduDataFieldLength;

    auto pduDataFieldLengthBytes = intToBeBytes<uint16_t>(realPduDataFieldLength);

    encodedHeader.insert(encodedHeader.end(), pduDataFieldLengthBytes.begin(),
                         pduDataFieldLengthBytes.end());
    encodedHeader.push_back(((uint8_t)segmentationControl << SEGMENTATION_CONTROL_POSITION) |
                            ((lengthOfEntityIDs - 1) << 4) | ((uint8_t)segmentMetadataFlag << 3) |
                            ((lengthOfTransactionSequenceNumber - 1) << 0));

    auto sourceEntityIDBytes = intToBeBytes<EntityIDType>(sourceEntityID);

    encodedHeader.insert(encodedHeader.end(), sourceEntityIDBytes.begin(),
                         sourceEntityIDBytes.end());

    auto transactionSequenceNumberBytes =
        intToBeBytes<SequenceNumberType>(transactionSequenceNumber);

    encodedHeader.insert(encodedHeader.end(), transactionSequenceNumberBytes.begin(),
                         transactionSequenceNumberBytes.end());

    auto destinationEntityIDBytes = intToBeBytes<EntityIDType>(destinationEntityID);

    encodedHeader.insert(encodedHeader.end(), destinationEntityIDBytes.begin(),
                         destinationEntityIDBytes.end());

    return encodedHeader;
};
