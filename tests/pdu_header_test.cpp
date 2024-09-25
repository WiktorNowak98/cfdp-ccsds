#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp/pdu_enums.hpp>
#include <cfdp/pdu_header.hpp>

#include <array>

using ::testing::ElementsAreArray;

using ::cfdp::pdu::header::CrcFlag;
using ::cfdp::pdu::header::Direction;
using ::cfdp::pdu::header::LargeFileFlag;
using ::cfdp::pdu::header::PduHeader;
using ::cfdp::pdu::header::PduType;
using ::cfdp::pdu::header::SegmentationControl;
using ::cfdp::pdu::header::SegmentMetadataFlag;
using ::cfdp::pdu::header::TransmissionMode;

class PduHeaderTest : public testing::Test
{
  protected:
    constexpr static std::array<uint8_t, 7> encoded_header_frame{51, 0, 5, 0, 1, 1, 2};
};

TEST_F(PduHeaderTest, TestHeaderEncoding)
{
    auto header =
        PduHeader(1, PduType::FileData, Direction::TowardsReceiver, TransmissionMode::Acknowledged,
                  CrcFlag::CrcPresent, LargeFileFlag::LargeFile, 1,
                  SegmentationControl::BoundariesNotPreserved, 1, SegmentMetadataFlag::NotPresent,
                  1, 1, 1, 2);

    auto encoded = header.encodeToBytes();

    ASSERT_EQ(encoded.size(), 4 + 3);
    EXPECT_THAT(encoded, ElementsAreArray(encoded_header_frame));
}

TEST_F(PduHeaderTest, TestHeaderDecoding)
{
    auto encodedHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end()};
    auto header = PduHeader(encodedHeaderView);

    ASSERT_EQ(header.getRawSize(), 7);
}
