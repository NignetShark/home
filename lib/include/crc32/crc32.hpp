//
// Created by paul on 22/03/2021.
//

#ifndef HOME_CRC32_HPP
#define HOME_CRC32_HPP

#include <stdint.h>

class CRC32
{
private:
    explicit CRC32();
    uint32_t table[256];

public:
    static CRC32& get();

    uint32_t update(uint32_t initial, const void* buf, size_t len);

};


#endif //HOME_CRC32_HPP
