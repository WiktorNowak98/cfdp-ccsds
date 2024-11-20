#include "pdu_enums.hpp"
#include "pdu_interface.hpp"

#include <cstdint>
#include <span>

using ::cfdp::pdu::header::LargeFileFlag;

namespace cfdp::pdu::directive
{

class KeepAlive : PduInterface
{
  public:
    KeepAlive(uint64_t progress, LargeFileFlag largeFileFlag);
    KeepAlive(std::span<uint8_t const> memory);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

    [[nodiscard]] inline uint16_t getRawSize() const override
    {
        return (largeFileFlag == LargeFileFlag::LargeFile) ? const_large_file_size_bytes
                                                           : const_small_file_size_bytes;
    };

    uint64_t progress;
    LargeFileFlag largeFileFlag;

  private:
    static constexpr uint16_t const_small_file_size_bytes = sizeof(uint8_t) + sizeof(uint32_t);
    static constexpr uint16_t const_large_file_size_bytes = sizeof(uint8_t) + sizeof(uint64_t);

    [[nodiscard]] inline uint8_t getProgressSize() const
    {
        return (largeFileFlag == LargeFileFlag::LargeFile) ? sizeof(uint64_t) : sizeof(uint32_t);
    }
};

class Ack : PduInterface
{
  public:
    Ack(Directive directiveCode, Condition conditionCode, TransactionStatus transactionStatus);
    Ack(std::span<uint8_t const> memory);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;
    [[nodiscard]] inline uint16_t getRawSize() const override { return const_pdu_size_bytes; };

    Directive directiveCode;
    DirectiveSubtype directiveSubtype;
    Condition conditionCode;
    TransactionStatus transactionStatus;

  private:
    static constexpr uint16_t const_pdu_size_bytes = sizeof(uint8_t) + sizeof(uint16_t);
};

class EndOfFile : PduInterface
{
  public:
    EndOfFile(Condition conditionCode, uint32_t checksum, uint64_t fileSize,
              LargeFileFlag largeFileFlag, uint8_t lengthOfEntityID, uint64_t faultEntityID);
    EndOfFile(Condition conditionCode, uint32_t checksum, uint64_t fileSize,
              LargeFileFlag largeFileFlag);
    EndOfFile(std::span<uint8_t const> memory, LargeFileFlag largeFileFlag);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

    [[nodiscard]] inline uint16_t getRawSize() const override
    {
        return const_pdu_size_bytes + getSizeOfFileSize() + getFaultLocationSize();
    };

    uint64_t fileSize;
    LargeFileFlag largeFileFlag;
    Condition conditionCode;
    uint32_t checksum;
    uint8_t lengthOfEntityID = 0;
    uint64_t faultEntityID   = 0;

  private:
    static constexpr uint8_t const_pdu_size_bytes =
        sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t);

    [[nodiscard]] inline bool isError() const { return conditionCode != Condition::NoError; }

    [[nodiscard]] inline uint8_t getSizeOfFileSize() const
    {
        return (largeFileFlag == LargeFileFlag::LargeFile) ? sizeof(uint64_t) : sizeof(uint32_t);
    }

    [[nodiscard]] inline uint8_t getFaultLocationSize() const
    {
        return (isError()) ? sizeof(uint8_t) + sizeof(uint8_t) + lengthOfEntityID : 0;
    }
};
} // namespace cfdp::pdu::directive
