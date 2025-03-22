#ifndef SECTORIZED_BLOOM_H
#define SECTORIZED_BLOOM_H

#include "Bitmap.h"

#include <stdio.h>
#include <cstdint>

class SectorizedBloom {
public:
SectorizedBloom(size_t bitsPerEntry, size_t numEntries, size_t wordsPerBlock);
    void insert(int64_t entry);
    bool query(int64_t entry);

private:
    size_t bitsPerEntry_;
    size_t numEntries_;
    // Must be power of 2
    // Aka number of sectors
    size_t wordsPerBlock_;
    // Size of register * wordsPerBlock
    // Used to compute hash within block
    size_t blockSize_;
    size_t size_;
    // Used to select block
    size_t numBlocks_;
    // Rounded up to be multiple of words per block
    size_t numHashFuncs_;
    size_t hashesPerSector_;
    Bitmap bitmap_;

    size_t getFirstSectorIndex(int64_t entry);
};

#endif