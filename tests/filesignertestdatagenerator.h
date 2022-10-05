#ifndef FILESIGNERTESTDATAGENERATOR_H
#define FILESIGNERTESTDATAGENERATOR_H

#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "randomizer.h"

#include "../src/hashrot13.h"
#include "../src/signaturefile.h"

class TestDataGenerator
{
public:
    using TestDataType  = std::tuple<std::string, SignatureFile::HashArray>;

    TestDataGenerator(size_t block_size)
        : block_size_(block_size)
    {
    }

    TestDataType GenTestDataForFileSizeEqualZero(const std::string& input_file_name)
    {
        std::ofstream(input_file_name, std::ios::binary | std::ios::trunc);
        return {input_file_name, {}};
    }

    TestDataType GenTestDataForFileSizeEqualBlockSize(const std::string& input_file_name)
    {
        std::vector<char> block(block_size_);
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
            item = randomizer.Random();
        });

        auto hash = HashRot13<SignatureFile::HashType>(std::begin(block), std::end(block));
        SignatureFile::HashArray expected_signature(1, hash);

        std::ofstream file(input_file_name, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));

        return {input_file_name, expected_signature};
    }

    TestDataType GenTestDataForFileSizeLessBlockSize(const std::string& input_file_name)
    {
        size_t data_size  = block_size_ / 2;

        std::vector<char> block(data_size, 0);
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
            item = randomizer.Random();
        });

        auto hash = HashRot13<SignatureFile::HashType>(std::begin(block), std::end(block));
        SignatureFile::HashArray expected_signature(1, hash);

        std::ofstream file(input_file_name, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));

        return {input_file_name, expected_signature};
    }

    TestDataType GenTestDataForFileSizeGreaterBlockSize(const std::string& input_file_name)
    {
        size_t data_size  = block_size_ + block_size_ / 2;

        std::vector<char> data(data_size, 0);
        Randomizer<char> randomizer(0);
        std::for_each(std::begin(data), std::end(data), [&](auto&& item) {
            item = randomizer.Random();
        });

        SignatureFile::HashArray expected_signature(2, 0);

        expected_signature.at(0) = HashRot13<SignatureFile::HashType>(std::begin(data), std::begin(data) + block_size_);
        expected_signature.at(1) = HashRot13<SignatureFile::HashType>(std::begin(data) + block_size_, std::end(data));

        std::ofstream file(input_file_name, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(data), std::end(data), std::ostream_iterator<char>(file));

        return {input_file_name, expected_signature};
    }

    TestDataType GenTestDataForFileHasManyBlocks(const std::string& input_file_name)
    {
        size_t data_size  = 1000 * block_size_ + block_size_ - 1;

        std::vector<char> data(block_size_, 0);

        if (!std::filesystem::exists(input_file_name)) {
            Randomizer<char> randomizer(0);
            std::ofstream input(input_file_name, std::ios::binary);
            for (size_t i = 0; i < data_size; ++i) {
                auto index = i % block_size_;
                data[index] = randomizer.Random();
                if (index == block_size_ - 1 || i == data_size - 1) {
                    auto size = i == data_size - 1 ? index : data.size();
                    if (!input.write(data.data(), size)) {
                        throw std::runtime_error("Cannot write to input file: " + input_file_name);
                    }
                }
            }
        }

        std::ifstream input(input_file_name, std::ios::binary);

        auto block_count = data_size / block_size_ + (data_size % block_size_ != 0 ? 1 : 0);
        SignatureFile::HashArray expected_signature;
        for (size_t i = 0; i < block_count; ++i) {
            if (!input.read(data.data(), block_size_) && !input.eof()) {
                throw std::runtime_error("Cannot read from input file: " + input_file_name);
            }
            auto begin = std::begin(data);
            auto end = input.gcount() < block_size_ ? begin + input.gcount() : std::end(data);
            expected_signature.push_back(HashRot13<SignatureFile::HashType>(begin, end));
        }

        return {input_file_name, expected_signature};
    }

    std::map<std::string_view, TestDataType> GenerateTestData()
    {
        return {
            { kFileSizeEqualZero,        GenTestDataForFileSizeEqualZero("input0.bin")        },
            { kFileSizeEqualBlockSize,   GenTestDataForFileSizeEqualBlockSize("input1.bin")   },
            { kFileSizeLessBlockSize,    GenTestDataForFileSizeLessBlockSize("input2.bin")    },
            { kFileSizeGreaterBlockSize, GenTestDataForFileSizeGreaterBlockSize("input3.bin") },
            { kFileHasManyBlocks,        GenTestDataForFileHasManyBlocks("input4.bin")        }
        };
    }

    static constexpr std::string_view kFileSizeEqualZero        = "FileSizeEqualZero";
    static constexpr std::string_view kFileSizeEqualBlockSize   = "FileSizeEqualBlockSize";
    static constexpr std::string_view kFileSizeLessBlockSize    = "FileSizeLessBlockSize";
    static constexpr std::string_view kFileSizeGreaterBlockSize = "FileSizeGreaterBlockSize";
    static constexpr std::string_view kFileHasManyBlocks        = "FileHasManyBlocks";

private:
    size_t block_size_ = 0;
};


#endif // FILESIGNERTESTDATAGENERATOR_H
