#include <cfdp/pdu_header.hpp>
#include <gtest/gtest.h>

namespace cfdp::pdu::header
{
TEST(PduHeader, Basic)
{
    auto header =
        PduHeader(1, PduType::FileData, Direction::TowardsReceiver, TransmissionMode::Acknowledged,
                  CrcFlag::CrcPresent, LargeFileFlag::LargeFile, 1,
                  SegmentationControl::BoundariesNotPreserved, 1, SegmentMetadataFlag::NotPresent,
                  1, 1, 1, 2);

    auto encoded = header.encodeToBytes();
}
} // namespace cfdp::pdu::header
