#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <limits>
#include <random>

template<typename T>
class Randomizer final
{
public:
    Randomizer(T min, T max = std::numeric_limits<T>::max())
        : _generator(_random_device())
        , _distrib(min, max)
    {

    }
    T Random()
    {
        return _distrib(_generator);
    }
protected:
    std::random_device _random_device;
    std::mt19937 _generator;
    std::uniform_int_distribution<T> _distrib;
};

#endif // RANDOMIZER_H
