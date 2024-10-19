#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_core/pdu_directive.hpp>
#include <cfdp_core/pdu_enums.hpp>
#include <cfdp_core/pdu_exceptions.hpp>

#include <cstdint>
#include <span>

using ::cfdp::pdu::exception::DecodeFromBytesException;
using ::cfdp::pdu::exception::PduConstructionException;

using ::cfdp::pdu::directive::AckPdu;
using ::cfdp::pdu::directive::Condition;
using ::cfdp::pdu::directive::Directive;
using ::cfdp::pdu::directive::EndOfFile;
using ::cfdp::pdu::directive::KeepAlivePdu;
using ::cfdp::pdu::directive::TransactionStatus;
using ::cfdp::pdu::header::LargeFileFlag;

class KeepAlivePduTest : public testing::Test
{
  protected:
    static constexpr std::array<uint8_t, 9> encoded_large_frame = {12,  255, 255, 255, 255,
                                                                   255, 255, 255, 255};
    static constexpr std::array<uint8_t, 5> encoded_small_frame = {12, 255, 255, 255, 255};
};

class AckPduTest : public testing::Test
{
  protected:
    static constexpr std::array<uint8_t, 3> encoded_eof_ack_frame      = {6, 64, 34};
    static constexpr std::array<uint8_t, 3> encoded_finished_ack_frame = {6, 81, 34};
};

class EndOfFileTest : public testing::Test
{
  protected:
    static constexpr std::array<uint8_t, 10> encoded_small_no_error_frame = {
        4, 0, 66, 58, 53, 199, 255, 255, 255, 255};
    static constexpr std::array<uint8_t, 13> encoded_small_with_error_frame = {
        4, 96, 66, 58, 53, 199, 255, 255, 255, 255, 6, 48, 57};
    static constexpr std::array<uint8_t, 14> encoded_large_no_error_frame = {
        4, 0, 66, 58, 53, 199, 255, 255, 255, 255, 255, 255, 255, 255};
    static constexpr std::array<uint8_t, 17> encoded_large_with_error_frame = {
        4, 96, 66, 58, 53, 199, 255, 255, 255, 255, 255, 255, 255, 255, 6, 48, 57};
    ;
};

TEST_F(KeepAlivePduTest, TestEncodingSmallFile)
{
    auto pdu     = KeepAlivePdu(UINT32_MAX);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::SmallFile);
    ASSERT_EQ(pdu.getRawSize(), sizeof(uint8_t) + sizeof(uint32_t));
    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_small_frame));
}

TEST_F(KeepAlivePduTest, TestEncodingLargeFile)
{
    auto pdu     = KeepAlivePdu(UINT64_MAX);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::LargeFile);
    ASSERT_EQ(pdu.getRawSize(), sizeof(uint8_t) + sizeof(uint64_t));
    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_large_frame));
}

TEST_F(KeepAlivePduTest, TestDecodingSmallFile)
{
    auto encoded = std::span<uint8_t const>{encoded_small_frame.begin(), encoded_small_frame.end()};

    auto pdu = KeepAlivePdu(encoded);

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::SmallFile);
    ASSERT_EQ(pdu.getProgress(), UINT32_MAX);
}

TEST_F(KeepAlivePduTest, TestDecodingLargeFile)
{
    auto encoded = std::span<uint8_t const>{encoded_large_frame.begin(), encoded_large_frame.end()};

    auto pdu = KeepAlivePdu(encoded);

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::LargeFile);
    ASSERT_EQ(pdu.getProgress(), UINT64_MAX);
}

TEST_F(KeepAlivePduTest, TestDecodingTooShortByteStream)
{
    auto encoded =
        std::span<uint8_t const>{encoded_large_frame.begin(), encoded_large_frame.end() - 1};

    ASSERT_THROW(KeepAlivePdu{encoded}, cfdp::pdu::exception::DecodeFromBytesException);
}

TEST_F(KeepAlivePduTest, TestDecodingWrongDirectiveCode)
{
    std::array<uint8_t, 5> encoded_frame = {13, 255, 255, 255, 255};

    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end() - 1};

    ASSERT_THROW(KeepAlivePdu{encoded}, cfdp::pdu::exception::DecodeFromBytesException);
}

TEST_F(AckPduTest, TestConstructorException)
{
    ASSERT_THROW(
        AckPdu(Directive::Ack, Condition::KeepAliveLimitReached, TransactionStatus::Terminated),
        PduConstructionException);
}

TEST_F(AckPduTest, TestEncodingEofAck)
{
    auto pdu =
        AckPdu(Directive::Eof, Condition::KeepAliveLimitReached, TransactionStatus::Terminated);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getDirectiveCode(), Directive::Eof);
    ASSERT_EQ(pdu.getConditionCode(), Condition::KeepAliveLimitReached);
    ASSERT_EQ(pdu.getTransactionStatus(), TransactionStatus::Terminated);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_eof_ack_frame));
}

TEST_F(AckPduTest, TestEncodingFinishedAck)
{
    auto pdu     = AckPdu(Directive::Finished, Condition::KeepAliveLimitReached,
                          TransactionStatus::Terminated);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getDirectiveCode(), Directive::Finished);
    ASSERT_EQ(pdu.getConditionCode(), Condition::KeepAliveLimitReached);
    ASSERT_EQ(pdu.getTransactionStatus(), TransactionStatus::Terminated);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_finished_ack_frame));
}

TEST_F(AckPduTest, TestDecodingEofAck)
{
    auto encoded =
        std::span<uint8_t const>{encoded_eof_ack_frame.begin(), encoded_eof_ack_frame.end()};

    auto pdu = AckPdu(encoded);

    ASSERT_EQ(pdu.getDirectiveCode(), Directive::Eof);
    ASSERT_EQ(pdu.getConditionCode(), Condition::KeepAliveLimitReached);
    ASSERT_EQ(pdu.getTransactionStatus(), TransactionStatus::Terminated);
}

TEST_F(AckPduTest, TestDecodingFinishedAck)
{
    auto encoded = std::span<uint8_t const>{encoded_finished_ack_frame.begin(),
                                            encoded_finished_ack_frame.end()};

    auto pdu = AckPdu(encoded);

    ASSERT_EQ(pdu.getDirectiveCode(), Directive::Finished);
    ASSERT_EQ(pdu.getConditionCode(), Condition::KeepAliveLimitReached);
    ASSERT_EQ(pdu.getTransactionStatus(), TransactionStatus::Terminated);
}

TEST_F(AckPduTest, TestDecodingWrongByteStreamSize)
{
    auto encoded =
        std::span<uint8_t const>{encoded_eof_ack_frame.begin(), encoded_eof_ack_frame.end() - 1};

    ASSERT_THROW(KeepAlivePdu{encoded}, cfdp::pdu::exception::DecodeFromBytesException);
}

TEST_F(AckPduTest, TestDecodingWrongDirectiveCode)
{
    std::array<uint8_t, 3> encoded_frame = {12, 81, 34};

    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end()};

    ASSERT_THROW(KeepAlivePdu{encoded}, cfdp::pdu::exception::DecodeFromBytesException);
}

TEST_F(EndOfFileTest, TestConstructorException)
{
    ASSERT_THROW(EndOfFile(Condition::FileSizeError, 1, static_cast<uint32_t>(1)),
                 PduConstructionException);

    ASSERT_THROW(EndOfFile(Condition::FileSizeError, 1, static_cast<uint64_t>(1)),
                 PduConstructionException);

    ASSERT_THROW(EndOfFile(Condition::NoError, 1, static_cast<uint32_t>(1), 1, 1),
                 PduConstructionException);

    ASSERT_THROW(EndOfFile(Condition::NoError, 1, static_cast<uint64_t>(1), 1, 1),
                 PduConstructionException);
}

TEST_F(EndOfFileTest, TestEncodingSmallFileWithNoError)
{
    auto pdu     = EndOfFile(Condition::NoError, 1111111111, UINT32_MAX);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::SmallFile);
    ASSERT_EQ(pdu.getConditionCode(), Condition::NoError);
    ASSERT_EQ(pdu.getFileSize(), UINT32_MAX);
    ASSERT_EQ(pdu.getChecksum(), 1111111111);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 0);
    ASSERT_EQ(pdu.getFaultEntityID(), 0);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_small_no_error_frame));
}

TEST_F(EndOfFileTest, TestEncodingLargeFileWithNoError)
{
    auto pdu     = EndOfFile(Condition::NoError, 1111111111, UINT64_MAX);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::LargeFile);
    ASSERT_EQ(pdu.getConditionCode(), Condition::NoError);
    ASSERT_EQ(pdu.getFileSize(), UINT64_MAX);
    ASSERT_EQ(pdu.getChecksum(), 1111111111);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 0);
    ASSERT_EQ(pdu.getFaultEntityID(), 0);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_large_no_error_frame));
}

TEST_F(EndOfFileTest, TestEncodingSmallFileWithError)
{
    auto pdu     = EndOfFile(Condition::FileSizeError, 1111111111, UINT32_MAX, 2, 12345);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getConditionCode(), Condition::FileSizeError);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 2);
    ASSERT_EQ(pdu.getFaultEntityID(), 12345);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_small_with_error_frame));
}

TEST_F(EndOfFileTest, TestEncodingLargeFileWithError)
{
    auto pdu     = EndOfFile(Condition::FileSizeError, 1111111111, UINT64_MAX, 2, 12345);
    auto encoded = pdu.encodeToBytes();

    ASSERT_EQ(pdu.getConditionCode(), Condition::FileSizeError);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 2);
    ASSERT_EQ(pdu.getFaultEntityID(), 12345);

    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_large_with_error_frame));
}

TEST_F(EndOfFileTest, TestDecodingSmallFileWithNoError)
{
    auto encoded = std::span<uint8_t const>{encoded_small_no_error_frame.begin(),
                                            encoded_small_no_error_frame.end()};

    auto pdu = EndOfFile(encoded, LargeFileFlag::SmallFile, 0);

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::SmallFile);
    ASSERT_EQ(pdu.getConditionCode(), Condition::NoError);
    ASSERT_EQ(pdu.getFileSize(), UINT32_MAX);
    ASSERT_EQ(pdu.getChecksum(), 1111111111);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 0);
    ASSERT_EQ(pdu.getFaultEntityID(), 0);
}

TEST_F(EndOfFileTest, TestDecodingLargeFileWithNoError)
{
    auto encoded = std::span<uint8_t const>{encoded_large_no_error_frame.begin(),
                                            encoded_large_no_error_frame.end()};

    auto pdu = EndOfFile(encoded, LargeFileFlag::LargeFile, 0);

    ASSERT_EQ(pdu.getLargeFileFlag(), LargeFileFlag::LargeFile);
    ASSERT_EQ(pdu.getConditionCode(), Condition::NoError);
    ASSERT_EQ(pdu.getFileSize(), UINT64_MAX);
    ASSERT_EQ(pdu.getChecksum(), 1111111111);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 0);
    ASSERT_EQ(pdu.getFaultEntityID(), 0);
}

TEST_F(EndOfFileTest, TestDecodingSmallFileWithError)
{
    auto encoded = std::span<uint8_t const>{encoded_small_with_error_frame.begin(),
                                            encoded_small_with_error_frame.end()};

    auto pdu = EndOfFile(encoded, LargeFileFlag::SmallFile, 2);

    ASSERT_EQ(pdu.getConditionCode(), Condition::FileSizeError);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 2);
    ASSERT_EQ(pdu.getFaultEntityID(), 12345);
}

TEST_F(EndOfFileTest, TestDecodinglargeFileWithError)
{
    auto encoded = std::span<uint8_t const>{encoded_large_with_error_frame.begin(),
                                            encoded_large_with_error_frame.end()};

    auto pdu = EndOfFile(encoded, LargeFileFlag::LargeFile, 2);

    ASSERT_EQ(pdu.getConditionCode(), Condition::FileSizeError);
    ASSERT_EQ(pdu.getLengthOfEntityID(), 2);
    ASSERT_EQ(pdu.getFaultEntityID(), 12345);
}

TEST_F(EndOfFileTest, TestDecodingWrongByteStreamSize)
{
    auto encoded = std::span<uint8_t const>{encoded_small_no_error_frame.begin(),
                                            encoded_small_no_error_frame.end() - 1};

    ASSERT_THROW(EndOfFile(encoded, LargeFileFlag::SmallFile, 0), DecodeFromBytesException);
}

TEST_F(EndOfFileTest, TestDecodingWrongTLVType)
{
    std::array<uint8_t, 13> encoded_frame = {4, 96, 66, 58, 53, 199, 255, 255, 255, 255, 5, 48, 57};
    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end()};

    ASSERT_THROW(EndOfFile(encoded, LargeFileFlag::SmallFile, 0), DecodeFromBytesException);
}
