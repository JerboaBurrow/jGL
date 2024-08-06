#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <stdint.h>

namespace jGL
{
    template <class C>
    std::vector<std::byte> ptrToByteVector(const C * ch, uint64_t length)
    {
        std::vector<std::byte> v(length);
        for (unsigned i = 0; i < length; i++)
        {
            v[i] = std::byte(ch[i]);
        }
        return v;
    }   
}

#endif /* UTIL_H */
