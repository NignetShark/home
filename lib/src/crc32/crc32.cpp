//
// Created by paul on 23/03/2021.
//
#include <cstddef>
#include "crc32/crc32.hpp"

CRC32::CRC32() {
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (size_t j = 0; j < 8; j++)
        {
            if (c & 1) {
                c = polynomial ^ (c >> 1);
            }
            else {
                c >>= 1;
            }
        }
        this->table[i] = c;
    }
}

uint32_t CRC32::update(uint32_t initial, const void *buf, size_t len) {
    uint32_t c = initial ^ 0xFFFFFFFF;
    const uint8_t* u = static_cast<const uint8_t*>(buf);
    for (size_t i = 0; i < len; ++i)
    {
        c = this->table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}

CRC32 &CRC32::get() {
    static CRC32 instance = CRC32();
    return instance;
}

