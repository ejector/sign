#include <gtest/gtest.h>

#include "../src/filesigner.h"

TEST(FileSigner, GenerateSign)
{
    FileSigner file_signer;

    EXPECT_EQ(false, file_signer.GenerateSign("", "", 1));
}
