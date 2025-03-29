#ifndef BLOCKED_BLOOM_H
#define BLOCKED_BLOOM_H

#include "Bitmap.h"
#include "Filter.h"

#include <stdio.h>
#include <cstdint>

class BlockedBloom : public Filter {
public:
    BlockedBloom(size_t bitsPerEntry, size_t numEntries, size_t wordsPerBlock);
    void insert(int64_t entry);
    bool query(int64_t entry);

    size_t bitsPerEntry_;
    size_t numEntries_;
    // Must be power of 2
    size_t wordsPerBlock_;
    // Size of register * wordsPerBlock
    // Used to compute hash within block
    size_t blockSize_;
    size_t size_;
    // Used to select block
    size_t numBlocks_;
    size_t numHashFuncs_;
    Bitmap bitmap_;

    size_t getBlockIndex(int64_t entry);
};

#endif