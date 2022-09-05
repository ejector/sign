#ifndef FILESIGNER_H
#define FILESIGNER_H

#include <string>

class FileSigner
{
    using FileName = std::string;
public:
    [[nodiscard]] bool GenerateSign(const FileName& input, const FileName& output, size_t block_size);
    [[nodiscard]] const std::string& GetErrorString() const
    {
        return error_string_;
    }

protected:
    std::string error_string_;
};

#endif // FILESIGNER_H
