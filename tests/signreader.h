#ifndef SIGNREADER_H
#define SIGNREADER_H

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

class SignReader final
{
public:
    using HashType = unsigned int;
    using HashArray = std::vector<HashType>;
public:
    static HashArray Read(const std::string& file_name)
    {
        std::ifstream file(file_name, std::ios::binary);

        if (!file.is_open()) {
            throw std::logic_error("Cannot open file!");
        }

        HashArray signature;
        HashType item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(item))) {
            signature.push_back(item);
        }

        return signature;
    }
};

#endif // SIGNREADER_H
