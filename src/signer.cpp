#include "signer.h"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <cstring>

#include "hashrot13.h"
#include "taskpool.h"
#include "signaturefile.h"
#include "pool.h"

bool Signer::GenerateSign(const FileName& input, const FileName& output, size_t block_size) noexcept
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

        std::atomic_bool aborted = false;
        Pool<std::ifstream> stream_pool(ThreadPool::Instance().MaximumThreadCount());

        auto GenerateHash = [&](size_t block_index) {

                auto stream_ptr = stream_pool.Get();

                if (!stream_ptr) {
                    throw std::logic_error("Stream does not exist!");
                }

                if (!stream_ptr->is_open()) {
                    stream_ptr->exceptions(std::ios::failbit);
                    stream_ptr->open(input, std::ios::binary);
                }

                auto position = block_index * block_size;
                stream_ptr->seekg(position);

                SignatureFile::HashType hash{};
                auto remainder = file_size - position;
                auto read_size =  remainder < block_size ? remainder : block_size;
                char ch;
                while (!aborted && read_size-- != 0 && stream_ptr->read(&ch, sizeof(ch))) {
                    hash = HashRot13(hash, ch);
                }

                return hash;
        };

        std::string error;

        {
            std::mutex lock;
            TaskPool task_pool;

            for (decltype(blocks_count) i = 0; i < blocks_count; ++i) {
                task_pool.Exec([&, i = i]() {
                    try {
                        signature.at(i) = GenerateHash(i);
                    } catch (const std::exception& e) {
                        std::lock_guard locker(lock);
                        if (!aborted) {
                            error = e.what();
                            if (auto* ios_failure = dynamic_cast<const std::ios_base::failure*>(&e)) {
                                error += std::string(": ") + std::strerror(ios_failure->code().value());
                            }
                        }
                        aborted = true;
                    }
                });
            }
        }

        if (aborted) {
            throw std::logic_error(error);
        }

        SignatureFile signature_file(output);
        signature_file.Write(signature);

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
