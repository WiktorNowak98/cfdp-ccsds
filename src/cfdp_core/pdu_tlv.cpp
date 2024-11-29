#include "cfdp_core/pdu_tlv.hpp"
#include "cfdp_core/pdu_enums.hpp"
#include <cfdp_core/pdu_exceptions.hpp>
#include <cfdp_core/utils.hpp>
#include <cstdint>
#include <span>
#include <vector>

namespace utils     = ::cfdp::utils;
namespace exception = ::cfdp::pdu::exception;

cfdp::pdu::tlv::EntityId::EntityId(std::span<uint8_t const> memory)
{

    const auto memory_size = memory.size();

    if (memory_size < sizeof(uint8_t) + sizeof(uint8_t))
    {
        throw exception::DecodeFromBytesException("Passed memory does not contain enough bytes");
    }

    if (memory[0] != utils::toUnderlying(TLVType::EntityId))
    {
        throw exception::DecodeFromBytesException("TLVType is not Enitity Id");
    }

    lengthOfEntityID = memory[1];

    if (memory_size < sizeof(uint8_t) + sizeof(uint8_t) + lengthOfEntityID)
    {
        throw exception::DecodeFromBytesException("Passed memory does not contain enough bytes");
    }

    faultEntityID = utils::bytesToInt<uint64_t>(memory, 2, lengthOfEntityID);
}

std::vector<uint8_t> cfdp::pdu::tlv::EntityId::encodeToBytes() const
{
    const auto pdu_size = getRawSize();
    auto encodedTlv     = std::vector<uint8_t>{};

    encodedTlv.reserve(pdu_size);

    encodedTlv.push_back(utils::toUnderlying(TLVType::EntityId));
    encodedTlv.push_back(lengthOfEntityID);

    auto faultEntityIDBytes = utils::intToBytes(faultEntityID, lengthOfEntityID);
    utils::concatenateVectorsInplace(faultEntityIDBytes, encodedTlv);

    return encodedTlv;
}
