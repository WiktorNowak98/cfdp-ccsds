#include <cfdp_core/pdu_directive.hpp>
#include <cfdp_core/pdu_enums.hpp>
#include <cfdp_core/pdu_exceptions.hpp>
#include <cfdp_core/utils.hpp>

#include <cstdint>
#include <span>
#include <vector>

namespace header    = ::cfdp::pdu::header;
namespace utils     = ::cfdp::utils;
namespace exception = ::cfdp::pdu::exception;

using ::cfdp::pdu::header::LargeFileFlag;

cfdp::pdu::directive::KeepAlivePdu::KeepAlivePdu(uint64_t progress)
    : progress(progress), largeFileFlag(LargeFileFlag::LargeFile){};

cfdp::pdu::directive::KeepAlivePdu::KeepAlivePdu(uint32_t progress)
    : progress(progress), largeFileFlag(LargeFileFlag::SmallFile){};

cfdp::pdu::directive::KeepAlivePdu::KeepAlivePdu(std::span<uint8_t const> memory)
{
    const auto memory_size = memory.size();

    if (memory_size < const_small_file_size_bytes)
    {
        throw exception::DecodeFromBytesException("Passed memory does not contain enough bytes");
    }

    if (memory[0] != (uint8_t)Directive::KeepAlive)
    {
        throw exception::DecodeFromBytesException("File Directive code is not Keep Alive Pdu");
    }

    largeFileFlag = (memory_size > const_small_file_size_bytes) ? LargeFileFlag::LargeFile
                                                                : LargeFileFlag::SmallFile;
    progress      = utils::bytesToInt<uint64_t>(
        memory, 1,
        (largeFileFlag == LargeFileFlag::LargeFile) ? sizeof(uint64_t) : sizeof(uint32_t));
};

std::vector<uint8_t> cfdp::pdu::directive::KeepAlivePdu::encodeToBytes() const
{
    const auto pdu_size = getRawSize();
    auto encodedPdu     = std::vector<uint8_t>{};

    encodedPdu.reserve(pdu_size);

    encodedPdu.push_back(static_cast<uint8_t>(Directive::KeepAlive));

    auto progressBytes =
        utils::intToBytes(progress, (largeFileFlag == LargeFileFlag::LargeFile) ? sizeof(uint64_t)
                                                                                : sizeof(uint32_t));
    utils::concatenateVectorsInplace(progressBytes, encodedPdu);

    return encodedPdu;
}
