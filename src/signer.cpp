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

        if (block_size == 0) {
            throw std::logic_error("Block size must be greater then zero!");
        }

        auto file_size = std::filesystem::file_size(input);

        if (file_size == 0) {
            std::ofstream ostream(output, std::ios::binary | std::ios::trunc);
            return true;
        }

        auto block_count = file_size / block_size + (file_size % block_size != 0 ? 1 : 0);

        SignatureFile::HashArray signature(block_count, 0);

        auto GenerateHash = [&](size_t block_index) {
            std::ifstream stream(input, std::ios::binary);
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

            for (decltype(block_count) i = 0; i < block_count; ++i) {
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
