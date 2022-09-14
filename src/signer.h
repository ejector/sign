#ifndef SIGNER_H
#define SIGNER_H

#include <string>

class Signer
{
    using FileName = std::string;
public:
    [[nodiscard]] bool GenerateSign(const FileName& input, const FileName& output, size_t block_size) noexcept;
    [[nodiscard]] const std::string& GetErrorString() const
    {
        return error_string_;
    }

protected:
    void CheckBlockSize(size_t block_size);
    void CheckOutputFile(const FileName& value);
    void CreateEmptyFile(const FileName& value);
    size_t GetBlocksCount(size_t file_size, size_t block_size);

protected:
    std::string error_string_;
};

#endif // SIGNER_H
