#include "BloomFilter.h"
#include "Utils.h"
#include "xxhash.h"

#include <stdlib.h>
#include <new>
#include <cmath>

BloomFilter::BloomFilter(size_t bitsPerEntry, size_t numEntries)
    : bitsPerEntry_(bitsPerEntry)
    , numEntries_(numEntries)
    , size_(Utils::roundUp(bitsPerEntry_ * numEntries_, Utils::BITMAP_UNIT))
    , bitmap_(size_)
    , numHashFuncs_(static_cast<size_t>(std::round(std::log(2) * bitsPerEntry_))) {

    }

void BloomFilter::insert(int64_t entry) {
    for (size_t i = 0; i < numHashFuncs_; i++) {
        size_t index = xxh::xxhash3<64>(&entry, sizeof(int64_t), i) % size_;
        bitmap_.set(index);
    }
}

bool BloomFilter::query(int64_t entry) {
    for (size_t i = 0; i < numHashFuncs_; i++) {
        size_t index = xxh::xxhash3<64>(&entry, sizeof(int64_t), i) % size_;
        if (!bitmap_.read(index)) return false;
    }

    return true;
}
