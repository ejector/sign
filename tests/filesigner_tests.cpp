#ifndef FILESIGNER_TESTS_H
#define FILESIGNER_TESTS_H

#include <filesystem>

#include <gtest/gtest.h>

#include "filesignertestdatagenerator.h"

#include "../src/signer.h"
#include "../src/signaturefile.h"

class FileSignerTest : public ::testing::Test
{
public:
    static const std::map<std::string_view, TestDataGenerator::TestDataType>& FileSignerTestData()
    {
        static auto test_data = TestDataGenerator(kBlockSize).GenerateTestData();
        return test_data;
    }
protected:
    void TearDown() override
    {
        std::filesystem::remove(kOutpuFileName);
    }
protected:
    static inline constexpr size_t kBlockSize = 1 * 1024 * 1024;
    const std::string kOutpuFileName = "output.sign";
};

TEST_F(FileSignerTest, GenerateTestData)
{
    FileSignerTest::FileSignerTestData();
}

TEST_F(FileSignerTest, GenerateSignNegative)
{
    Signer file_signer;

    ASSERT_EQ(false, file_signer.GenerateSign("", "", 1));
    ASSERT_NE("", file_signer.GetErrorString());
}

TEST_F(FileSignerTest, GenerateSignWhenBlockSizeEqualZero)
{
    Signer file_signer;

    ASSERT_EQ(false, file_signer.GenerateSign("", "", 0));
    ASSERT_NE("", file_signer.GetErrorString());
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeEqualZero)
{
    auto&& [input_file_name, expected_signature] = FileSignerTestData().at(TestDataGenerator::kFileSizeEqualZero);

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeEqualBlockSize)
{
    auto&& [input_file_name, expected_signature] = FileSignerTestData().at(TestDataGenerator::kFileSizeEqualBlockSize);

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeLessBlockSize)
{
    auto&& [input_file_name, expected_signature] = FileSignerTestData().at(TestDataGenerator::kFileSizeLessBlockSize);

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeGreaterBlockSize)
{
    auto&& [input_file_name, expected_signature] = FileSignerTestData().at(TestDataGenerator::kFileSizeGreaterBlockSize);

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileHasManyBlocks)
{
    auto&& [input_file_name, expected_signature] = FileSignerTestData().at(TestDataGenerator::kFileHasManyBlocks);

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, CleanTestData)
{
    if (bool remove_generated_files = false; remove_generated_files) {
        for (auto&& [test_name, test_data]: FileSignerTest::FileSignerTestData()) {
            auto&& [file_name, hash_array] = test_data;
            EXPECT_TRUE(std::filesystem::remove(file_name));
        }
    }
}

#endif // FILESIGNER_TESTS_H
