#include "filesigner.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "hashrot13.h"

bool FileSigner::GenerateSign(const FileName& input, const FileName& output, size_t block_size)
{
    try {
        auto file_size = std::filesystem::file_size(input);
        auto block_count = file_size / block_size;

        std::cout << "Block counts: " << block_count << std::endl;

        std::vector<unsigned int> signature(block_count, 0);

        auto GenerateHash = [=](size_t block_index) {
            std::ifstream stream(input, std::ios::binary);
            if (!stream) {
                throw std::logic_error("Cannot open file: " + input);
            }

            auto position = block_index * block_size;
            stream.seekg(position);

            std::istreambuf_iterator<char> iterator(stream);
            return HashRot13(iterator, block_size);
        };

        for (decltype(block_count) i = 0; i < block_count; ++i) {
            std::cout << "Block Count: " << i << std::endl;
            signature.at(i) = GenerateHash(i);
        }

        std::ofstream ostream(output, std::ios::binary | std::ios::trunc);

        for (const auto& item: signature) {
            ostream << item;
        }

        return true;

    } catch (std::exception& e) {
        error_string_ = e.what();
    }

    return false;
}
