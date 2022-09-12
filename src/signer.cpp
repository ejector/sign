#include "signer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#include "hashrot13.h"
#include "taskpool.h"
#include "signaturefile.h"
#include "pool.h"

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

        Pool<std::ifstream> stream_pool(ThreadPool::Instance().MaximumThreadCount(), input, std::ios::binary);

        auto GenerateHash = [&](size_t block_index) {
            auto stream_ptr = stream_pool.Get();

            if (!stream_ptr) {
                throw std::logic_error("Stream does not exist!");
            }

            auto position = block_index * block_size;

            if (!stream_ptr->seekg(position)) {
                throw std::logic_error("Cannot read file: " + output + ". Error: " + std::strerror(errno));
            }

            std::istreambuf_iterator<char> iterator(*stream_ptr), end;
            return HashRot13<SignatureFile::HashType>(iterator, end, block_size);
        };

        {
            TaskPool task_pool;

            for (decltype(blocks_count) i = 0; i < blocks_count; ++i) {
                task_pool.WaitIfFullAndExec([&, i = i]() {
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
