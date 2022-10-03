#ifndef STREAMHASHGENERATOR_H
#define STREAMHASHGENERATOR_H

#include <atomic>
#include <fstream>
#include <string>

#include "hashrot13.h"
#include "pool.h"
#include "signaturefile.h"

class StreamHashGenerator final
{
public:
    StreamHashGenerator(const std::string& file_name,
                        size_t file_size,
                        size_t block_size,
                        size_t pool_size)
        : stream_pool_(pool_size),
          file_name_(file_name),
          file_size_(file_size),
          block_size_(block_size)
    {
    }

    SignatureFile::HashType Generate(size_t block_index)
    {
        auto stream_ptr = stream_pool_.Get();

        if (!stream_ptr) {
            throw std::logic_error("Stream does not exist!");
        }

        if (!stream_ptr->is_open()) {
            stream_ptr->exceptions(std::ios::failbit);
            stream_ptr->open(file_name_, std::ios::binary);
        }

        auto position = block_index * block_size_;
        stream_ptr->seekg(position);

        SignatureFile::HashType hash{};
        auto remainder = file_size_ - position;
        auto read_size =  remainder < block_size_ ? remainder : block_size_;
        char ch;
        while (!aborted_ && read_size-- != 0 && stream_ptr->read(&ch, sizeof(ch))) {
            hash = HashRot13(hash, ch);
        }

        return hash;
    }

    bool Aborted() const
    {
        return aborted_;
    }

    void SetAborted(bool value)
    {
        aborted_ = value;
    }

private:
    Pool<std::ifstream> stream_pool_;
    std::string file_name_;
    size_t file_size_ = 0;
    size_t block_size_ = 0;
    std::atomic_bool aborted_ = false;
};

#endif // STREAMHASHGENERATOR_H
