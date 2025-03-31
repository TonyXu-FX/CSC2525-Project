#include "SectorizedBloom.h"
#include "Utils.h"
#include "xxhash.h"

#include <stdlib.h>
#include <new>
#include <cmath>

SectorizedBloom::SectorizedBloom(size_t bitsPerEntry, size_t numEntries, size_t wordsPerBlock)
    : bitsPerEntry_(bitsPerEntry)
    , numEntries_(numEntries)
    , wordsPerBlock_(wordsPerBlock)
    , blockSize_(Utils::REGISTER_BITS * wordsPerBlock_)
    , size_(Utils::roundUp(bitsPerEntry_ * numEntries_, blockSize_))
    , numBlocks_(static_cast<size_t>(size_ / blockSize_))
    , numHashFuncs_(Utils::roundUp(static_cast<size_t>(std::round(std::log(2) * bitsPerEntry_)) - 1, wordsPerBlock_) + 1)
    , hashesPerSector_((numHashFuncs_ - 1) / wordsPerBlock_)
    , bitmap_(size_) {
    }

void SectorizedBloom::insert(int64_t entry) {
    size_t sectorIndex = getFirstSectorIndex(entry);
    size_t seed = 1;
    for (size_t sectorOffset = 0; sectorOffset < wordsPerBlock_; sectorOffset++) {
        uint64_t mask = 0;
        for (size_t i = 1; i <= hashesPerSector_; i++) {
            size_t indexInSector = xxh::xxhash3<64>(&entry, sizeof(int64_t), seed) % Utils::REGISTER_BITS;
            mask |= (1ULL << indexInSector);
            seed++;
        }
        
        bitmap_.setSector(sectorIndex + sectorOffset, mask);
    }
}

bool SectorizedBloom::query(int64_t entry) {
    size_t sectorIndex = getFirstSectorIndex(entry);
    size_t seed = 1;
    for (size_t sectorOffset = 0; sectorOffset < wordsPerBlock_; sectorOffset++) {
        uint64_t mask = 0;
        for (size_t i = 1; i <= hashesPerSector_; i++) {
            size_t indexInSector = xxh::xxhash3<64>(&entry, sizeof(int64_t), seed) % Utils::REGISTER_BITS;
            mask |= (1ULL << indexInSector);
            seed++;
        }
        
        if (!bitmap_.readSector(sectorIndex + sectorOffset, mask)) return false;
    }

    return true;
}

size_t SectorizedBloom::getFirstSectorIndex(int64_t entry) {
    size_t blockNum = xxh::xxhash3<64>(&entry, sizeof(int64_t), 0) % numBlocks_;
    return blockNum * wordsPerBlock_;
}
