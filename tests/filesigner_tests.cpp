#include <fstream>

#include <gtest/gtest.h>

#include "../src/hashrot13.h"
#include "../src/filesigner.h"

#include <random>

template<typename T>
class Randomizer
{
public:
    Randomizer(T min, T max = std::numeric_limits<T>::max())
        : _generator(_random_device())
        , _distrib(min, max)
    {

    }
    int random()
    {
        return _distrib(_generator);
    }
protected:
    std::random_device _random_device;
    std::mt19937 _generator;
    std::uniform_int_distribution<> _distrib;
};

TEST(FileSigner, GenerateSignNegative)
{
    FileSigner file_signer;

    ASSERT_EQ(false, file_signer.GenerateSign("", "", 1));
    ASSERT_NE("", file_signer.GetErrorString());
}

TEST(FileSigner, GenerateSignWhenFileSizeEqualBlockSize)
{
    constexpr size_t block_size = 1 * 1024 * 1024;

    std::array<char, block_size> block;
    Randomizer<char> randomizer(0);
    std::for_each(std::begin(block), std::end(block), [&](auto&& item) {
        item = randomizer.random();
    });


    auto hash = HashRot13(std::begin(block), block_size);
    auto hash2 = HashRot13(std::begin(block), block_size);
    ASSERT_EQ(hash, hash2);

    const std::string kInputFileName = "input.bin";
    const std::string kOutpuFileName = "output.bin";
    {
        std::ofstream file(kInputFileName, std::ios::binary | std::ios::trunc);
        std::copy(std::begin(block), std::end(block), std::ostream_iterator<char>(file));
    }

    FileSigner file_signer;
    ASSERT_EQ(true, file_signer.GenerateSign(kInputFileName, kOutpuFileName, block_size));

    std::ifstream file_sign(kOutpuFileName, std::ios::binary);
    ASSERT_EQ(true, file_sign.is_open());

    decltype(hash) hash_from_file;
    file_sign >> hash_from_file;

    ASSERT_EQ(hash, hash_from_file);
}
