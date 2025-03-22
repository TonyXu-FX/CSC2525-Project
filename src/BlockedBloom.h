#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "Bitmap.h"

#include <stdio.h>
#include <cstdint>

class BlockedBloom {
public:
    BlockedBloom(size_t bitsPerEntry, size_t numEntries, size_t wordsPerBlock);
    void insert(int64_t entry);
    bool query(int64_t entry);

private:
    size_t bitsPerEntry_;
    size_t numEntries_;
    // Must be power of 2
    size_t wordsPerBlock_;
    // Size of register * wordsPerBlock
    size_t blockSize_;
    size_t size_;
    size_t numBlocks_;
    size_t numHashFuncs_;
    Bitmap bitmap_;

    size_t getBlockIndex(int64_t entry);
};

#endif