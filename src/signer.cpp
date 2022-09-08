#include "signer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "hashrot13.h"
#include "asyncpool.h"
#include "signaturefile.h"

bool Signer::GenerateSign(const FileName& input, const FileName& output, size_t block_size)
{
    try {

        CheckOutputFile(output);

        auto file_size = std::filesystem::file_size(input);

        if (file_size == 0) {
            CreateEmptyFile(output);
            return true;
        }

        auto blocks_count = GetBlocksCount(file_size, block_size);

        SignatureFile::HashArray signature(blocks_count, 0);

        auto GenerateHash = [&](size_t block_index) {
            std::ifstream stream(input, std::ios::binary);
            stream.exceptions(std::ios::failbit);

            if (!stream) {
                throw std::logic_error("Cannot open file: " + input);
            }

            auto position = block_index * block_size;
            stream.seekg(position);

            std::istreambuf_iterator<char> iterator(stream), end;
            return HashRot13<SignatureFile::HashType>(iterator, end, block_size);
        };

        {
            AsyncPool async_pool;

            for (decltype(blocks_count) i = 0; i < blocks_count; ++i) {
                async_pool.WaitIfFullAndExec([&, i = i]() {
                    signature.at(i) = GenerateHash(i);
                });
            }
        }

        SignatureFile sign_reader(output);
        sign_reader.Write(signature);

        return true;

    } catch (std::exception& e) {
        error_string_ = e.what();
    }

    return false;
}

void Signer::CheckOutputFile(const FileName& value)
{
    if (std::filesystem::exists(value)) {
        throw std::logic_error("File " + value + " already exists!");
    }
}

void Signer::CreateEmptyFile(const FileName& value)
{
    std::ofstream ostream(value, std::ios::binary | std::ios::trunc);
    if (!ostream) {
        throw std::logic_error("Cannot open file: " + value);
    }
}

size_t Signer::GetBlocksCount(size_t file_size, size_t block_size)
{
    CheckBlockSize(block_size);
    return file_size / block_size + (file_size % block_size != 0 ? 1 : 0);
}

void Signer::CheckBlockSize(size_t block_size)
{
    if (block_size == 0) {
        throw std::logic_error("Block size must be greater then zero!");
    }
}

