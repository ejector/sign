#ifndef HASHROT13_H
#define HASHROT13_H

#include <cstddef>
#include <iostream>

template<typename Iter>
inline unsigned int HashRot13(Iter iter, size_t size)
{
    unsigned int hash = 0;

    size_t counter = 0;

    auto end = Iter{};
    while (size-- != 0 && iter != end)
    {
        counter++;
        hash += *(iter++);
        hash -= (hash << 13) | (hash >> 19);
    }

    std::cout << "counter " << counter << ", hash " << hash << std::endl;

    return hash;
}

#endif // HASHROT13_H
