#include "unistd.h"
#include "BloomFilter.h"
#include "BlockedBloom.h"
#include "SectorizedBloom.h"
#include "Filter.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>
#include <list>
#include <chrono>
#include <iomanip>
#include <cmath>

bool bloomFilterTests() {
    std::cout << "===Bloom Filter Tests===" << std::endl;
    BloomFilter bloom(12, 1000);
    if (bloom.numHashFuncs_ != 9) {
        std::cout << "Wrong num hash functions" << std::endl;
        return false;
    }
    for (int64_t i = 0; i < 1000; i++) {
        bloom.insert(2*i);
    }

    for (int64_t i = 0; i < 1000; i++) {
        if (!bloom.query(2*i)) {
            std::cout << "False Negative" << std::endl;
            return false;
        }
    }

    size_t falsePositives = 0;
    for (int64_t i = 0; i < 10000; i++) {
        if (bloom.query(2*i + 1)) {
            falsePositives++;
        }
    }
    std::cout << "FPR: " << static_cast<double>(falsePositives) / 100000 << std::endl;
    std::cout << "===Bloom Filter Tests Succeeded===" << std::endl;
    return true;
}

bool blockedBloomTests() {
    std::cout << "===Blocked Bloom Filter Tests===" << std::endl;
    BlockedBloom blockedBloom(12, 1000, 8);
    if (blockedBloom.numHashFuncs_ != 9) {
        std::cout << "Wrong num hash functions" << std::endl;
        return false;
    }
    if (blockedBloom.blockSize_ != 512) {
        std::cout << "Wrong num blocks" << std::endl;
        return false;
    }
    for (int64_t i = 0; i < 1000; i++) {
        blockedBloom.insert(2*i);
    }

    for (int64_t i = 0; i < 1000; i++) {
        if (!blockedBloom.query(2*i)) {
            std::cout << "False Negative" << std::endl;
            return false;
        }
    }

    size_t falsePositives = 0;
    for (int64_t i = 0; i < 10000; i++) {
        if (blockedBloom.query(2*i + 1)) {
            falsePositives++;
        }
    }
    std::cout << "FPR: " << static_cast<double>(falsePositives) / 100000 << std::endl;
    std::cout << "===Blocked Bloom Filter Tests Succeeded===" << std::endl;
    return true;
}

bool sectorizedBloomTests() {
    std::cout << "===Sectorized Bloom Filter Tests===" << std::endl;
    std::cout << "---Standard Queries---" << std::endl;
    SectorizedBloom sectorizedBloom(12, 1000, 8);
    if (sectorizedBloom.numHashFuncs_ != 9) {
        std::cout << "Wrong num hash functions" << std::endl;
        return false;
    }
    if (sectorizedBloom.blockSize_ != 512) {
        std::cout << "Wrong block size" << std::endl;
        return false;
    }
    if (sectorizedBloom.hashesPerSector_ != 1) {
        std::cout << "Wrong hashes per sector" << std::endl;
        return false;
    }

    for (int64_t i = 0; i < 1000; i++) {
        sectorizedBloom.insert(2*i);
    }

    for (int64_t i = 0; i < 1000; i++) {
        if (!sectorizedBloom.query(2*i)) {
            std::cout << "False Negative" << std::endl;
            return false;
        }
    }

    size_t falsePositives = 0;
    for (int64_t i = 0; i < 10000; i++) {
        if (sectorizedBloom.query(2*i + 1)) {
            falsePositives++;
        }
    }
    std::cout << "FPR: " << static_cast<double>(falsePositives) / 100000 << std::endl;

    std::cout << "---Checking Sectorization---" << std::endl;
    SectorizedBloom testBloom(11, 1000, 4);
    testBloom.insert(0);
    testBloom.insert(1);
    testBloom.insert(2);
    size_t setSectorCount = 0;
    for (size_t i = 0; i < testBloom.numBlocks_ * testBloom.wordsPerBlock_; i++) {
        if (testBloom.bitmap_.bitmap_[i] != 0) setSectorCount++;
    }
    assert(setSectorCount == 12); // 4 words per block => 12 words total should be set

    std::cout << "===Sectorized Bloom Filter Tests Succeeded===" << std::endl;
    return true;
}

/**
 * Test the insertion throughput of the filter
 * Also serves to populate the filter
 */
int throughputExp(std::unique_ptr<Filter> filter, size_t numEntries) {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t entry = 0; entry < numEntries; entry++) {
        filter->insert(entry << 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    double throughput = numEntries / elapsed;
    filter.reset();
    return throughput;
}

/*int fixedNumEntriesFprExp(std::unique_ptr<Filter> filter) {

}*/

void fixedNumEntriesExps() {
    std::ofstream throughputFile("fixedEntriesThroughput_results.csv");
    if (!throughputFile.is_open()) {
        std::cerr << "Error: Unable to open the output file!" << std::endl;
        return; // Exit early if file can't be opened
    }

    throughputFile << "Bits/Entry;Bloom;Blocked Bloom;Sectorized Bloom" << std::endl;

    size_t numEntries = 8 * 1024 * 1024;
    for (size_t bitsPerEntry = 6; bitsPerEntry <= 36; bitsPerEntry += 6) {
        std::cout << "---Testing Bloom---" << std::endl;
        std::unique_ptr<BloomFilter> bloom = std::make_unique<BloomFilter>(bitsPerEntry, numEntries);
        double throughputBloom = throughputExp(std::move(bloom), numEntries);
        std::cout << "---Testing Bloom Done---" << std::endl;

        std::cout << "---Testing Blocked Bloom---" << std::endl;
        std::unique_ptr<BlockedBloom> blockedBloom = std::make_unique<BlockedBloom>(bitsPerEntry, numEntries, 4);
        double throughputBlocked = throughputExp(std::move(blockedBloom), numEntries);
        std::cout << "---Testing Blocked Bloom Done---" << std::endl;

        std::cout << "---Testing Sectorized Bloom---" << std::endl;
        std::unique_ptr<SectorizedBloom> sectorizedBloom = std::make_unique<SectorizedBloom>(bitsPerEntry, numEntries, 4);
        double throughputSectorized = throughputExp(std::move(sectorizedBloom), numEntries);
        std::cout << "---Testing Sectorized Bloom Done---" << std::endl;

        throughputFile << std::fixed << std::setprecision(1)
                << bitsPerEntry << ";"
                << throughputBloom << ";"
                << throughputBlocked << ";"
                << throughputSectorized << std::endl;
    }
    throughputFile.close();
}

void fixedBpeExps() {
    std::ofstream outfile("fixedBPE_results.csv");
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open the output file!" << std::endl;
        return; // Exit early if file can't be opened
    }

    outfile << "DataSize(KB);Bloom;Blocked Bloom;Sectorized Bloom" << std::endl;

    size_t bitsPerEntry = 12; // 9 hash functions
    for (int i = 1; i <= 8; i++) {
        size_t numEntries = 75'000 * static_cast<size_t>(pow(2, i));
        std::unique_ptr<BloomFilter> bloom = std::make_unique<BloomFilter>(bitsPerEntry, numEntries);
        size_t dataSizeKB = bloom->bitmap_.size_ / (1024 * 8);

        std::cout << "---Testing Bloom---" << std::endl;
        double throughputBloom = throughputExp(std::move(bloom), numEntries);
        std::cout << "---Testing Bloom Done---" << std::endl;

        std::cout << "---Testing Blocked Bloom---" << std::endl;
        std::unique_ptr<BlockedBloom> blockedBloom = std::make_unique<BlockedBloom>(bitsPerEntry, numEntries, 8);
        double throughputBlocked = throughputExp(std::move(blockedBloom), numEntries);
        std::cout << "---Testing Blocked Bloom Done---" << std::endl;

        std::cout << "---Testing Sectorized Bloom---" << std::endl;
        std::unique_ptr<SectorizedBloom> sectorizedBloom = std::make_unique<SectorizedBloom>(bitsPerEntry, numEntries, 8);
        double throughputSectorized = throughputExp(std::move(sectorizedBloom), numEntries);
        std::cout << "---Testing Sectorized Bloom Done---" << std::endl;

        outfile << std::fixed << std::setprecision(1)
                << dataSizeKB << ";"
                << throughputBloom << ";"
                << throughputBlocked << ";"
                << throughputSectorized << std::endl;
    }
    outfile.close();
}

void runTests() {
    assert(bloomFilterTests());
    assert(blockedBloomTests());
    assert(sectorizedBloomTests());
}

void runExps() {
    fixedBpeExps();
    fixedNumEntriesExps();
}

int main() {
    //runTests();
    runExps();    

    return 0;
}