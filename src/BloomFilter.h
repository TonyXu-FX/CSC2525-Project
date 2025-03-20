#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stdio.h>
#include <cstdint>

class BloomFilter {
public:
    BloomFilter(size_t bitsPerEntry, size_t numEntries);
    ~BloomFilter();
    void insert(int64_t entry);
    bool query(int64_t entry);

private:
    uint8_t* bitmap_;
    size_t bitsPerEntry_;
    size_t numEntries_;
    // Rounded up to multiple of 8
    size_t size_;
    size_t numHashFuncs_;

    // Bit vector is declared as an array
    const static size_t BITMAP_UNIT = 8;
};

#endif