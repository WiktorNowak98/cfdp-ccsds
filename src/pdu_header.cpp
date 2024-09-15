#include <cfdp/pdu_header.hpp>

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> || std::unsigned_integral<SequenceNumberType>
size_t cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::getRawSize()
{
    return CONST_HEADER_SIZE_BYTES + sizeof(EntityIDType) + sizeof(SequenceNumberType) +
           sizeof(EntityIDType);
};

template <class EntityIDType, class SequenceNumberType>
    requires std::unsigned_integral<EntityIDType> || std::unsigned_integral<SequenceNumberType>
std::vector<uint8_t> cfdp::pdu::PduHeader<EntityIDType, SequenceNumberType>::encodeToBytes()
{
    auto headerSize    = getRawSize();
    auto encodedHeader = std::vector<uint8_t>{};

    encodedHeader.reserve(headerSize);

    return encodedHeader;
};
