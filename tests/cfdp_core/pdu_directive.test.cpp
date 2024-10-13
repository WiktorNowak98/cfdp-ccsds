#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cfdp_core/pdu_directive.hpp>
#include <cfdp_core/pdu_enums.hpp>
#include <cfdp_core/pdu_exceptions.hpp>

#include <cstdint>
#include <span>

using ::cfdp::pdu::exception::DecodeFromBytesException;
using ::cfdp::pdu::exception::PduConstructionException;

using ::cfdp::pdu::directive::KeepAlivePdu;
using ::cfdp::pdu::header::LargeFileFlag;

class KeepAlivePduTest : public testing::Test
{
  protected:
    static constexpr std::array<uint8_t, 9> encoded_large_frame = {12,  255, 255, 255, 255,
                                                                   255, 255, 255, 255};
    static constexpr std::array<uint8_t, 5> encoded_small_frame = {12, 255, 255, 255, 255};
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

TEST_F(KeepAlivePduTest, TestDecodingWrongFireDirectiveCode)
{
    std::array<uint8_t, 5> encoded_frame = {13, 255, 255, 255, 255};

    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end() - 1};

    ASSERT_THROW(KeepAlivePdu{encoded}, cfdp::pdu::exception::DecodeFromBytesException);
}
