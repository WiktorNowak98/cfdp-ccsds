#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp/pdu_enums.hpp>
#include <cfdp/pdu_header.hpp>

#include <array>
#include <memory>

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

// TODO: 25.09.2024 <@uncommon-nickname>
// We need to change those assertions the the correct exceptions when
// we get access to the private headers. For now any throw will
// suffice.
TEST_P(PduHeaderConstructorTest, TestPduHeaderConstructorExceptions)
{
    auto [lengthOfEntityIDs, sourceEntityID, destEntityID, lengthOfTransaction, transactionNumber] =
        GetParam();
    ASSERT_ANY_THROW(buildHeader(lengthOfEntityIDs, sourceEntityID, destEntityID,
                                 lengthOfTransaction, transactionNumber));
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

TEST_F(PduHeaderTest, TestHeaderDecodingTooShortByteStream)
{
    auto incompleteHeaderView =
        std::span<uint8_t const>{encoded_header_frame.begin(), encoded_header_frame.end() - 3};

    ASSERT_ANY_THROW(PduHeader{incompleteHeaderView});
}
