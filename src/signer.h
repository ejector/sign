#ifndef SIGNER_H
#define SIGNER_H

#include <string>

class Signer final
{
    using FileName = std::string;
public:
    [[nodiscard]] bool GenerateSign(const FileName& input, const FileName& output, size_t block_size) noexcept;
    [[nodiscard]] const std::string& GetErrorString() const
    {
        return error_string_;
    }

protected:
    void CheckBlockSize(size_t block_size) const;
    void CheckOutputFile(const FileName& value) const;
    void CreateEmptyFile(const FileName& value) const;
    [[nodiscard]] size_t GetBlocksCount(size_t file_size, size_t block_size) const;

protected:
    std::string error_string_;
};

#endif // SIGNER_H
