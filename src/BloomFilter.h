#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "Bitmap.h"

#include <stdio.h>
#include <cstdint>

class BloomFilter {
public:
    BloomFilter(size_t bitsPerEntry, size_t numEntries);
    void insert(int64_t entry);
    bool query(int64_t entry);

private:
    size_t bitsPerEntry_;
    size_t numEntries_;
    // Rounded up to multiple of 8
    size_t size_;
    Bitmap bitmap_;
    size_t numHashFuncs_;
};

#endif