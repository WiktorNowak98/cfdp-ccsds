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

// TODO: 25.09.2024 <@uncommon-nickname>
// We need to change those assertions the the correct exceptions when
// we get access to the private headers. For now any throw will
// suffice.
namespace
{
constexpr std::array<uint8_t, 11> encoded_header_frame = {51, 1, 248, 4, 1, 0, 0, 0, 5, 150, 2};
}

TEST(PduHeaderTest, TestHeaderConstructionForbiddenLengthOfEntityIDs)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 0,
                               SegmentMetadataFlag::NotPresent, 5, 1, 1, 2));
}

TEST(PduHeaderTest, TestHeaderConstructionForbiddenLengthOfTransaction)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 1,
                               SegmentMetadataFlag::NotPresent, 0, 1, 1, 2));
}

TEST(PduHeaderTest, TestHeaderConstructionSourceEntityIDTooBigToFit)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 1,
                               SegmentMetadataFlag::NotPresent, 1, 1400, 1, 2));
}

TEST(PduHeaderTest, TestHeaderConstructionDestinationEntityIDTooBigToFit)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 1,
                               SegmentMetadataFlag::NotPresent, 1, 1, 1, 1400));
}

TEST(PduHeaderTest, TestHeaderConstructionTransactionTooBigToFit)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 1,
                               SegmentMetadataFlag::NotPresent, 1, 1, 1400, 1));
}

TEST(PduHeaderTest, TestHeaderConstructionSameEntityIDs)
{
    ASSERT_ANY_THROW(PduHeader(1, PduType::FileData, Direction::TowardsReceiver,
                               TransmissionMode::Acknowledged, CrcFlag::CrcPresent,
                               LargeFileFlag::LargeFile, 500,
                               SegmentationControl::BoundariesNotPreserved, 1,
                               SegmentMetadataFlag::NotPresent, 1, 1, 1, 1));
}

TEST(PduHeaderTest, TestHeaderEncoding)
{
    auto header =
        PduHeader(1, PduType::FileData, Direction::TowardsReceiver, TransmissionMode::Acknowledged,
                  CrcFlag::CrcPresent, LargeFileFlag::LargeFile, 500,
                  SegmentationControl::BoundariesNotPreserved, 1, SegmentMetadataFlag::NotPresent,
                  5, 1, 1430, 2);

    auto encoded = header.encodeToBytes();

    ASSERT_EQ(header.getRawSize(), 4 + (1 * 2) + 5);
    EXPECT_THAT(encoded, ElementsAreArray(encoded_header_frame));
}

TEST(PduHeaderTest, TestHeaderDecoding)
{
    auto encodedHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end()};

    auto header = PduHeader(encodedHeaderView);

    ASSERT_EQ(header.getVersion(), 1);
    ASSERT_EQ(header.getPduType(), PduType::FileData);
    ASSERT_EQ(header.getDirection(), Direction::TowardsReceiver);
    ASSERT_EQ(header.getTransmissionMode(), TransmissionMode::Acknowledged);
    ASSERT_EQ(header.getCrcFlag(), CrcFlag::CrcPresent);
    ASSERT_EQ(header.getLargeFileFlag(), LargeFileFlag::LargeFile);
    ASSERT_EQ(header.getPduDataFieldLength(), 500);
    ASSERT_EQ(header.getSegmentationControl(), SegmentationControl::BoundariesNotPreserved);
    ASSERT_EQ(header.getLengthOfEntityIDs(), 1);
    ASSERT_EQ(header.getSegmentMetadataFlag(), SegmentMetadataFlag::NotPresent);
    ASSERT_EQ(header.getLengthOfTransaction(), 5);
    ASSERT_EQ(header.getSourceEntityID(), 1);
    ASSERT_EQ(header.getTransactionNumber(), 1430);
    ASSERT_EQ(header.getDestinationEntityID(), 2);
}

TEST(PduHeaderTest, TestHeaderDecodingTooShortByteStream)
{
    auto incompleteHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end() - 3};

    ASSERT_ANY_THROW(PduHeader{incompleteHeaderView});
}
