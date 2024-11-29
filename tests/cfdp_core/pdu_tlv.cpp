#include "cfdp_core/pdu_tlv.hpp"
#include "cfdp_core/pdu_exceptions.hpp"
#include "gmock/gmock.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <span>

using ::cfdp::pdu::exception::DecodeFromBytesException;

using ::cfdp::pdu::tlv::EntityId;

class EntityIdTest : public testing::Test
{
  protected:
    static constexpr std::array<uint8_t, 8> encoded_frame = {6, 6, 0, 0, 0, 0, 4, 87};
};

TEST_F(EntityIdTest, TestEncoding)
{
    auto tlv     = EntityId(6, 1111);
    auto encoded = tlv.encodeToBytes();

    ASSERT_EQ(tlv.lengthOfEntityID, 6);
    ASSERT_EQ(tlv.faultEntityID, 1111);
    EXPECT_THAT(encoded, testing::ElementsAreArray(encoded_frame));
}

TEST_F(EntityIdTest, TestDecoding)
{
    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end()};
    auto tlv     = EntityId(encoded);

    ASSERT_EQ(tlv.lengthOfEntityID, 6);
    ASSERT_EQ(tlv.faultEntityID, 1111);
}

TEST_F(EntityIdTest, TestDecodingEmptyMemory)
{
    ASSERT_THROW(EntityId(std::span<uint8_t, 0>{}), DecodeFromBytesException);
}

TEST_F(EntityIdTest, TestDecodingTooSmallEntityLength)
{
    auto encoded = std::span<uint8_t const>{encoded_frame.begin(), encoded_frame.end() - 1};
    ASSERT_THROW(EntityId{encoded}, DecodeFromBytesException);
}
