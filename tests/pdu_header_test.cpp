#include "cfdp/pdu_header.hpp"
#include <gtest/gtest.h>

TEST(PduHeader, Basic)
{
    EXPECT_NE(cfdp::pdu::PduType::FileData, cfdp::pdu::PduType::FileDirective);
}
