#include "filesigner.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

template<typename Iter>
unsigned int HashRot13(Iter iter, size_t size)
{
    unsigned int hash = 0;

    auto end = Iter{};
    while (size-- != 0 && iter++ != end)
    {
        std::cout << "iter pos: " << size << ", value: " << *iter << std::endl;
        hash += *iter;
        hash -= (hash << 13) | (hash >> 19);
    }

    return hash;
}

bool FileSigner::GenerateSign(const FileName& input, const FileName& output, size_t block_size)
{
    auto file_size = std::filesystem::file_size(input);
    auto block_count = file_size / block_size;

    std::cout << "Block counts: " << block_count << std::endl;

    std::vector<unsigned int> signature(block_count, 0);

    auto GenerateHash = [=](size_t block_index) {
        std::ifstream stream(input);
        if (!stream) {
            throw std::logic_error("Cannot open file: " + input);
        }

        auto position = block_index * block_size;
        stream.seekg(position);

        std::istreambuf_iterator<char> iterator(stream);
        return HashRot13(iterator, block_size - 1);
    };

    for (decltype(block_count) i = 0; i < block_count; ++i) {
        signature.at(i) = GenerateHash(i);
    }

    return false;
}
