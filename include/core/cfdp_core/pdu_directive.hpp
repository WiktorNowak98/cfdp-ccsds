#include "cfdp_core/pdu_enums.hpp"
#include "cfdp_core/pdu_interface.hpp"
#include <cstdint>
#include <span>
namespace cfdp::pdu::directive
{

class KeepAlivePdu : PduInterface
{
  public:
    KeepAlivePdu(uint32_t progress);
    KeepAlivePdu(uint64_t progress);
    KeepAlivePdu(std::span<uint8_t const> memory);

    [[nodiscard]] std::vector<uint8_t> encodeToBytes() const override;
    [[nodiscard]] uint16_t getRawSize() const override
    {
        return (largeFileFlag == header::LargeFileFlag::LargeFile) ? const_large_file_size_bytes
                                                                   : const_small_file_size_bytes;
    };

    [[nodiscard]] auto getProgress() const { return progress; }
    [[nodiscard]] auto getLargeFileFlag() const { return largeFileFlag; }

  private:
    uint64_t progress;
    header::LargeFileFlag largeFileFlag;

    static constexpr uint16_t const_small_file_size_bytes = sizeof(uint8_t) + sizeof(uint32_t);
    static constexpr uint16_t const_large_file_size_bytes = sizeof(uint8_t) + sizeof(uint64_t);
};

} // namespace cfdp::pdu::directive
