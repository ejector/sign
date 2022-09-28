#include <array>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "randomizer.h"

#include "../src/hashrot13.h"
#include "../src/signer.h"
#include "../src/signaturefile.h"

class FileSignerTest : public ::testing::Test
{
protected:
    using TestDataType  = std::tuple<std::string, SignatureFile::HashArray>;

    void TearDown() override
    {
        std::filesystem::remove(kInputFileName);
        std::filesystem::remove(kOutpuFileName);
    }

    TestDataType GenTestDataForFileSizeEqualZero()
    {
        std::ofstream(kInputFileName, std::ios::binary | std::ios::trunc);
        return {kInputFileName, {}};
    }

    TestDataType GenTestDataForFileSizeEqualBlockSize()
    {
        std::array<char, kBlockSize> block;
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
            item = randomizer.random();
        });

        auto hash = HashRot13<SignatureFile::HashType>(std::begin(block), std::end(block));
        SignatureFile::HashArray expected_signature(1, hash);

        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));

        return {kInputFileName, expected_signature};
    }

    TestDataType GenTestDataForFileSizeLessBlockSize()
    {
        constexpr size_t data_size  = kBlockSize / 2;

        std::array<char, data_size> block;
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
            item = randomizer.random();
        });

        auto hash = HashRot13<SignatureFile::HashType>(std::begin(block), std::end(block));
        SignatureFile::HashArray expected_signature(1, hash);

        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));

        return {kInputFileName, expected_signature};
    }

    TestDataType GenTestDataForFileSizeGreaterBlockSize()
    {
        constexpr size_t data_size  = kBlockSize + kBlockSize / 2;

        std::array<char, data_size> data;
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(data), std::end(data), [&](auto&& item) {
            item = randomizer.random();
        });

        SignatureFile::HashArray expected_signature(2, 0);

        expected_signature.at(0) = HashRot13<SignatureFile::HashType>(std::begin(data), std::begin(data) + kBlockSize);
        expected_signature.at(1) = HashRot13<SignatureFile::HashType>(std::begin(data) + kBlockSize, std::end(data));

        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(data), std::end(data), std::ostream_iterator<char>(file));

        return {kInputFileName, expected_signature};
    }

    TestDataType GenTestDataForFileHasManyBlocks()
    {
        constexpr size_t data_size  = 1000 * kBlockSize + kBlockSize - 1;

        std::vector<char> data(data_size, 0);
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(data), std::end(data), [&](auto&& item) {
            item = randomizer.random();
        });

        auto block_count = data_size / kBlockSize + (data_size % kBlockSize != 0 ? 1 : 0);
        SignatureFile::HashArray expected_signature;
        for (size_t i = 0; i < block_count; ++i) {
            auto begin = std::begin(data) + i * kBlockSize;
            auto end = std::distance(begin, std::end(data)) < kBlockSize ? std::end(data) : begin + kBlockSize;
            expected_signature.push_back(HashRot13<SignatureFile::HashType>(begin, end));
        }

        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(data), std::end(data), std::ostream_iterator<char>(file));

        return {kInputFileName, expected_signature};
    }

    static constexpr size_t kBlockSize = 1 * 1024 * 1024;

    const std::string kInputFileName = "input.bin";
    const std::string kOutpuFileName = "output.sign";
};

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
    auto&& [input_file_name, expected_signature] = GenTestDataForFileSizeEqualZero();

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeEqualBlockSize)
{
    auto&& [input_file_name, expected_signature] = GenTestDataForFileSizeEqualBlockSize();

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeLessBlockSize)
{
    auto&& [input_file_name, expected_signature] = GenTestDataForFileSizeLessBlockSize();

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeGreaterBlockSize)
{
    auto&& [input_file_name, expected_signature] = GenTestDataForFileSizeGreaterBlockSize();

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileHasManyBlocks)
{
    auto&& [input_file_name, expected_signature] = GenTestDataForFileHasManyBlocks();

    Signer file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(input_file_name, kOutpuFileName, kBlockSize));

    auto actual_signature = SignatureFile::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}
