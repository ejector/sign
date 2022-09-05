#ifndef SIGNATUREFILE_H
#define SIGNATUREFILE_H

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

class SignatureFile final
{
public:
    using HashType = unsigned int;
    using HashArray = std::vector<HashType>;
public:
    [[nodiscard]] static HashArray Read(const std::string& file_name)
    {
        SignatureFile sign_reader(file_name);
        return sign_reader.Read();
    }
public:
    SignatureFile(const std::string& file_name)
        : file_name_(file_name)
    {

    }

    [[nodiscard]] HashArray Read() const
    {
        std::ifstream file(file_name_, std::ios::binary);

        if (!file.is_open()) {
            throw std::logic_error("Cannot open file!");
        }

        HashArray signature;
        HashType item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(HashType))) {
            signature.push_back(item);
        }

        return signature;
    }

    void Write(const HashArray& signature)
    {
        if (std::filesystem::exists(file_name_)) {
            throw std::logic_error("File already exists!");
        }

        std::ofstream file(file_name_, std::ios::binary);

        if (!file.is_open()) {
            throw std::logic_error("Cannot open file!");
        }

        for (const auto& item: signature) {
            file.write(reinterpret_cast<const char*>(&item), sizeof(HashType));
        }
    }
protected:
    std::string file_name_;
};

#endif // SIGNATUREFILE_H
