#ifndef HASHROT13_H
#define HASHROT13_H

#include <cstddef>
#include <iostream>

template<typename T, typename Iter>
inline T HashRot13(Iter begin, Iter end, size_t size = std::numeric_limits<size_t>::max())
{
    T hash = {};

    Iter iter = begin;
    while (size-- != 0 && iter != end)
    {
        hash += *(iter++);
        hash -= (hash << 13) | (hash >> 19);
    }

    return hash;
}


template<typename T, typename Add>
inline T HashRot13(T hash, Add add)
{
    hash += add;
    hash -= (hash << 13) | (hash >> 19);
    return hash;
}

#endif // HASHROT13_H
