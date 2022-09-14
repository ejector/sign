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
        HashArray signature;
        std::ifstream file;

        try {
            file.exceptions(std::ios::failbit);
            file.open(file_name_, std::ios::binary);

            HashType item;
            while (file.read(reinterpret_cast<char*>(&item), sizeof(HashType))) {
                signature.push_back(item);
            }
        } catch (const std::ios_base::failure& e) {
            if (!file.eof()) {
                throw;
            }
        }

        return signature;
    }

    void Write(const HashArray& signature)
    {
        std::ofstream file;
        file.exceptions(std::ios::failbit);

        file.open(file_name_, std::ios::binary);

        for (const auto& item: signature) {
            file.write(reinterpret_cast<const char*>(&item), sizeof(HashType));
        }
    }
protected:
    std::string file_name_;
};

#endif // SIGNATUREFILE_H
