#include "signer.h"

#include <filesystem>
#include <mutex>
#include <vector>
#include <cstring>

#include "signaturefile.h"
#include "streamhashgenerator.h"
#include "taskpool.h"

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

        StreamHashGenerator hash_generator(input, file_size, block_size, ThreadPool::Instance().MaximumThreadCount());
        {
            std::mutex lock;
            TaskPool task_pool;

            auto handle_exception = [&](const std::exception& e) {
                std::lock_guard locker(lock);
                if (!hash_generator.Aborted()) {
                    error_string_ = e.what();
                    if (const auto* ios_failure = dynamic_cast<const std::ios_base::failure*>(&e)) {
                        error_string_ += std::string(": ") + std::strerror(ios_failure->code().value());
                    }
                }
                hash_generator.SetAborted(true);
            };

            for (decltype(blocks_count) i = 0; i < blocks_count && !hash_generator.Aborted(); ++i) {
                task_pool.Exec([&, i = i]() {
                    try {
                        signature.at(i) = hash_generator.Generate(i);
                    } catch (const std::exception& e) {
                        handle_exception(e);
                    }
                });
            }
        }

        if (hash_generator.Aborted()) {
            return false;
        }

        SignatureFile signature_file(output);
        signature_file.Write(signature);

        return true;

    } catch (const std::exception& e) {
        error_string_ = e.what();
    }

    return false;
}

void Signer::CheckOutputFile(const FileName& value) const
{
    if (std::filesystem::exists(value)) {
        throw std::logic_error("File " + value + " already exists!");
    }
}

void Signer::CreateEmptyFile(const FileName& value) const
{
    std::ofstream ostream(value, std::ios::binary | std::ios::trunc);
    if (!ostream) {
        throw std::logic_error("Cannot open file: " + value);
    }
}

size_t Signer::GetBlocksCount(size_t file_size, size_t block_size) const
{
    CheckBlockSize(block_size);
    return file_size / block_size + (file_size % block_size != 0 ? 1 : 0);
}

void Signer::CheckBlockSize(size_t block_size) const
{
    if (block_size == 0) {
        throw std::logic_error("Block size must be greater then zero!");
    }
}
