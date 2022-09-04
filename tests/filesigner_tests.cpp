#include <fstream>

#include <gtest/gtest.h>

#include "randomizer.h"
#include "signreader.h"

#include "../src/hashrot13.h"
#include "../src/filesigner.h"

class FileSignerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    const std::string kInputFileName = "input.bin";
    const std::string kOutpuFileName = "output.sign";
};


TEST_F(FileSignerTest, GenerateSignNegative)
{
    FileSigner file_signer;

    ASSERT_EQ(false, file_signer.GenerateSign("", "", 1));
    ASSERT_NE("", file_signer.GetErrorString());
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeEqualZero)
{
    constexpr size_t block_size = 1 * 1024 * 1024;

    std::vector<unsigned int> expected_signature;

    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    auto actual_signature = SignReader::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeEqualBlockSize)
{
    constexpr size_t block_size = 1 * 1024 * 1024;

    std::array<char, block_size> block;
    Randomizer<char> randomizer(0);
    std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
        item = randomizer.random();
    });

    auto hash = HashRot13(std::begin(block), std::end(block));
    std::vector<unsigned int> expected_signature(1, hash);

    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    auto actual_signature = SignReader::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeLessBlockSize)
{
    constexpr size_t block_size = 1 * 1024 * 1024;
    constexpr size_t data_size  = block_size / 2;

    std::array<char, data_size> block;
    Randomizer<char> randomizer(0);
    std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
        item = randomizer.random();
    });

    auto hash = HashRot13(std::begin(block), std::end(block));
    std::vector<unsigned int> expected_signature(1, hash);

    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    std::ifstream file_sign(kOutpuFileName, std::ios::binary);
    ASSERT_EQ(true, file_sign.is_open());

    auto actual_signature = SignReader::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeGreaterBlockSize)
{
    constexpr size_t block_size = 1 * 1024 * 1024;
    constexpr size_t data_size  = block_size + block_size / 2;

    std::array<char, data_size> data;
    Randomizer<char> randomizer(0);
    std::for_each(std::begin(data), std::end(data), [&](auto&& item) {
        item = randomizer.random();
    });

    std::vector<unsigned int> expected_signature(2, 0);

    expected_signature.at(0) = HashRot13(std::begin(data), std::begin(data) + block_size);
    expected_signature.at(1) = HashRot13(std::begin(data) + block_size, std::end(data));

    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(data), std::end(data), std::ostream_iterator<char>(file));
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    auto actual_signature = SignReader::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}

TEST_F(FileSignerTest, GenerateSignWhenFileSizeManyBlockSize)
{
    constexpr size_t block_size = 1 * 1024 * 1024;
    constexpr size_t data_size  = 1000 * block_size + block_size - 1;

    std::vector<char> data(data_size, 0);
    Randomizer<char> randomizer(0);
    std::for_each(std::begin(data), std::end(data), [&](auto&& item) {
        item = randomizer.random();
    });

    auto block_count = data_size / block_size + (data_size % block_size != 0 ? 1 : 0);
    std::vector<unsigned int> expected_signature;
    for (size_t i = 0; i < block_count; ++i) {
        auto begin = std::begin(data) + i * block_size;
        auto end = std::distance(begin, std::end(data)) < block_size ? std::end(data) : begin + block_size;
        expected_signature.push_back(HashRot13(begin, end));
    }

    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(data), std::end(data), std::ostream_iterator<char>(file));
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    auto actual_signature = SignReader::Read(kOutpuFileName);
    ASSERT_EQ(expected_signature, actual_signature);
}
