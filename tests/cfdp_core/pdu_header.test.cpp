#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_core/pdu_enums.hpp>
#include <cfdp_core/pdu_exceptions.hpp>
#include <cfdp_core/pdu_header.hpp>

#include <array>
#include <functional>
#include <memory>
#include <tuple>

using ::testing::ElementsAreArray;

using ::cfdp::pdu::exception::DecodeFromBytesException;
using ::cfdp::pdu::exception::PduConstructionException;

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
  public:
    static constexpr std::array<uint8_t, 11> encoded_header_frame = {51, 1, 248, 4,   1, 0,
                                                                     0,  0, 5,   150, 2};
    std::unique_ptr<PduHeader> buildHeader(uint8_t lengthOfEntityIDs, uint64_t sourceEntityID,
                                           uint64_t destinationEntityID,
                                           uint8_t lengthOfTransaction, uint64_t transactionNumber)
    {
        return std::make_unique<PduHeader>(
            1, PduType::FileData, Direction::TowardsReceiver, TransmissionMode::Acknowledged,
            CrcFlag::CrcPresent, LargeFileFlag::LargeFile, 500,
            SegmentationControl::BoundariesNotPreserved, lengthOfEntityIDs,
            SegmentMetadataFlag::NotPresent, lengthOfTransaction, sourceEntityID, transactionNumber,
            destinationEntityID);
    }
};

class PduHeaderConstructorTest
    : public PduHeaderTest,
      public testing::WithParamInterface<std::tuple<uint8_t, uint64_t, uint64_t, uint8_t, uint64_t>>
{};

INSTANTIATE_TEST_SUITE_P(
    HeaderInitializerListConstructor, PduHeaderConstructorTest,
    testing::Values(std::make_tuple(0, 1, 2, 1, 1),    // lengthOfEntityIDs = 0
                    std::make_tuple(1, 1, 2, 0, 1),    // lengthOfTransaction = 0
                    std::make_tuple(1, 1400, 2, 1, 1), // sourceEntityID too big
                    std::make_tuple(1, 1, 1400, 1, 1), // destEntityID too big
                    std::make_tuple(1, 1, 2, 1, 1400), // transactionNumber too big
                    std::make_tuple(1, 1, 1, 1, 1)));  // sourceEntityID == destEntityID

TEST_P(PduHeaderConstructorTest, TestPduHeaderConstructorExceptions)
{
    auto params = GetParam();
    ASSERT_THROW(std::apply(std::bind_front(&PduHeaderConstructorTest::buildHeader, this), params),
                 PduConstructionException);
}

TEST_F(PduHeaderTest, TestHeaderEncoding)
{
    auto header  = buildHeader(1, 1, 2, 5, 1430);
    auto encoded = header->encodeToBytes();

    ASSERT_EQ(header->getRawSize(), 4 + (1 * 2) + 5);
    EXPECT_THAT(encoded, ElementsAreArray(encoded_header_frame));
}

TEST_F(PduHeaderTest, TestHeaderDecoding)
{
    auto encodedHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end()};

    auto header = PduHeader(encodedHeaderView);

    ASSERT_EQ(header.version, 1);
    ASSERT_EQ(header.pduType, PduType::FileData);
    ASSERT_EQ(header.direction, Direction::TowardsReceiver);
    ASSERT_EQ(header.transmissionMode, TransmissionMode::Acknowledged);
    ASSERT_EQ(header.crcFlag, CrcFlag::CrcPresent);
    ASSERT_EQ(header.largeFileFlag, LargeFileFlag::LargeFile);
    ASSERT_EQ(header.pduDataFieldLength, 500);
    ASSERT_EQ(header.segmentationControl, SegmentationControl::BoundariesNotPreserved);
    ASSERT_EQ(header.lengthOfEntityIDs, 1);
    ASSERT_EQ(header.segmentMetadataFlag, SegmentMetadataFlag::NotPresent);
    ASSERT_EQ(header.lengthOfTransaction, 5);
    ASSERT_EQ(header.sourceEntityID, 1);
    ASSERT_EQ(header.transactionSequenceNumber, 1430);
    ASSERT_EQ(header.destinationEntityID, 2);
}

TEST_F(PduHeaderTest, TestHeaderDecodingTooShortByteStream)
{
    auto incompleteHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end() - 3};

    ASSERT_THROW(PduHeader{incompleteHeaderView}, DecodeFromBytesException);
}
