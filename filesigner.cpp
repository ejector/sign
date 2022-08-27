#include "filesigner.h"

#include <filesystem>
#include <iostream>
#include <vector>

FileSigner::FileSigner()
{

}

bool FileSigner::GenerateSign(const FileName& input, const FileName& output, size_t block_size)
{
    auto file_size = std::filesystem::file_size(input);
    auto block_count = file_size / block_size;

    std::cout << "Block counts: " << block_count << std::endl;

    std::vector<unsigned int> signature(block_count, 0);

    auto GenerateHash = [=](size_t block_index) {
        return 0;
    };

    for (decltype(block_count) i = 0; i < block_count; ++i) {
        signature.at(i) = GenerateHash(i);
    }

    return false;
}
