#include "BlockedBloom.h"
#include "Utils.h"
#include "xxhash.h"

#include <stdlib.h>
#include <new>
#include <cmath>

BlockedBloom::BlockedBloom(size_t bitsPerEntry, size_t numEntries, size_t wordsPerBlock)
    : bitsPerEntry_(bitsPerEntry)
    , numEntries_(numEntries)
    , wordsPerBlock_(wordsPerBlock)
    , blockSize_(Utils::REGISTER_BITS * wordsPerBlock_)
    , size_(Utils::roundUp(bitsPerEntry_ * numEntries_, blockSize_))
    , numBlocks_(static_cast<size_t>(size_ / blockSize_))
    , numHashFuncs_(static_cast<size_t>(std::ceil(std::log(2) * bitsPerEntry_)))
    , bitmap_(size_) {
    }

void BlockedBloom::insert(int64_t entry) {
    size_t blockIndex = getBlockIndex(entry);
    for (size_t i = 1; i < numHashFuncs_; i++) {
        size_t blockOffset = Utils::modPow2(xxh::xxhash3<64>(&entry, sizeof(int64_t), i), blockSize_);
        bitmap_.set(blockIndex + blockOffset);
    }
}

bool BlockedBloom::query(int64_t entry) {
    size_t blockIndex = getBlockIndex(entry);
    for (size_t i = 1; i < numHashFuncs_; i++) {
        size_t blockOffset = Utils::modPow2(xxh::xxhash3<64>(&entry, sizeof(int64_t), i), blockSize_);
        if (!bitmap_.read(blockIndex + blockOffset)) return false;
    }

    return true;
}

size_t BlockedBloom::getBlockIndex(int64_t entry) {
    size_t blockNum = xxh::xxhash3<64>(&entry, sizeof(int64_t), 0) % numBlocks_;
    return blockNum * blockSize_;
}
