#include "pdu_enums.hpp"
#include "pdu_interface.hpp"

#include <cstdint>
#include <span>

using ::cfdp::pdu::header::LargeFileFlag;

namespace cfdp::pdu::directive
{

class KeepAlivePdu : PduInterface
{
  public:
    KeepAlivePdu(uint32_t progress) : progress(progress), largeFileFlag(LargeFileFlag::SmallFile){};
    KeepAlivePdu(uint64_t progress) : progress(progress), largeFileFlag(LargeFileFlag::LargeFile){};
    KeepAlivePdu(std::span<uint8_t const> memory);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;

    [[nodiscard]] inline uint16_t getRawSize() const override
    {
        return (largeFileFlag == LargeFileFlag::LargeFile) ? const_large_file_size_bytes
                                                           : const_small_file_size_bytes;
    };

    [[nodiscard]] auto getProgress() const { return progress; }
    [[nodiscard]] auto getLargeFileFlag() const { return largeFileFlag; }

  private:
    uint64_t progress;
    LargeFileFlag largeFileFlag;

    static constexpr uint16_t const_small_file_size_bytes = sizeof(uint8_t) + sizeof(uint32_t);
    static constexpr uint16_t const_large_file_size_bytes = sizeof(uint8_t) + sizeof(uint64_t);

    [[nodiscard]] inline uint8_t getProgressSize() const
    {
        return (largeFileFlag == LargeFileFlag::LargeFile) ? sizeof(uint64_t) : sizeof(uint32_t);
    }
};

} // namespace cfdp::pdu::directive
