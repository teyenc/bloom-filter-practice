#include <vector>
#include <string>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <cmath>
#include <iomanip>

/**
 * BloomFilter class implements a space-efficient probabilistic data structure
 * used to test whether an element is a member of a set.
 * False positive matches are possible, but false negatives are not.
 */
class BloomFilter {
private:
    std::vector<bool> bitArray;    // The bit array to store element presence
    size_t numHashFunctions;       // Number of hash functions to use

    /**
     * Custom hash function that generates different hashes based on a seed
     * Uses a simple but effective multiplication-based hash mixing
     * @param item The string to hash
     * @param seed The seed to generate different hash values for the same item
     * @return A hash value modulo the size of the bit array
     */
    size_t hash(const std::string& item, size_t seed) const {
        size_t hash = seed;
        for (char c : item) {
            hash ^= c;                  // XOR with current character
            hash *= 0x5bd1e995;         // Multiplication with a large prime
            hash ^= hash >> 15;         // Right shift to mix bits
        }
        return hash % bitArray.size();  // Ensure hash is within bit array bounds
    }

public:
    /**
     * Constructor initializes the Bloom filter with specified size and hash functions
     * @param size The size of the bit array
     * @param numHashes The number of hash functions to use
     */
    BloomFilter(size_t size, size_t numHashes)
            : bitArray(size, false), numHashFunctions(numHashes) {}

    /**
     * Inserts an item into the Bloom filter
     * @param item The string to insert
     */
    void insert(const std::string& item) {
        for (size_t i = 0; i < numHashFunctions; ++i) {
            bitArray[hash(item, i)] = true;  // Set bits at all hash positions
        }
    }

    /**
     * Checks if an item might be in the set
     * @param item The string to check
     * @return true if the item might be in the set, false if definitely not
     */
    bool mightContain(const std::string& item) const {
        for (size_t i = 0; i < numHashFunctions; ++i) {
            if (!bitArray[hash(item, i)]) {
                return false;  // If any bit is not set, item is definitely not in set
            }
        }
        return true;  // All bits set - item might be in set
    }

    /**
     * Calculates the actual memory usage in bytes
     * @return Memory usage in bytes
     */
    size_t getMemoryUsage() const {
        // Convert bits to bytes, rounding up for partial bytes
        return bitArray.size() / 8 + (bitArray.size() % 8 ? 1 : 0);
    }

    /**
     * Gets the size of the bit array
     * @return Size in bits
     */
    size_t getBitArraySize() const {
        return bitArray.size();
    }

    /**
     * Calculates optimal Bloom filter parameters based on expected elements and desired false positive rate
     * @param expectedElements Number of elements expected to be inserted
     * @param falsePositiveRate Desired false positive rate (e.g., 0.01 for 1%)
     * @return Pair of {optimal size, optimal number of hash functions}
     */
    static std::pair<size_t, size_t> optimalParameters(size_t expectedElements, double falsePositiveRate) {
        // Calculate optimal size: m = -(n * ln(p)) / (ln(2)^2)
        size_t m = ceil(-(expectedElements * log(falsePositiveRate)) / (log(2) * log(2)));
        // Calculate optimal number of hash functions: k = (m/n) * ln(2)
        size_t k = ceil((m / expectedElements) * log(2));
        return {m, k};
    }
};

/**
 * Structure to hold analysis results for comparing Bloom filter with HashSet
 */
struct AnalysisResult {
    double bloomMemoryKB;      // Memory used by Bloom filter in KB
    double hashSetMemoryKB;    // Memory used by HashSet in KB
    double savingsPercent;     // Percentage of memory saved
    double bitsPerItem;        // Average number of bits used per item
    double actualFPRate;       // Measured false positive rate
};

/**
 * Analyzes the performance of Bloom filter compared to HashSet
 * @param numItems Number of items to insert
 * @param targetFPRate Target false positive rate
 * @return Analysis results including memory usage and actual false positive rate
 */
AnalysisResult analyzeConfiguration(int numItems, double targetFPRate) {
    // Get optimal parameters and initialize data structures
    auto [bloomSize, numHashes] = BloomFilter::optimalParameters(numItems, targetFPRate);
    BloomFilter bloom(bloomSize, numHashes);
    std::unordered_set<std::string> hashSet;

    // Insert test data
    for (int i = 0; i < numItems; ++i) {
        std::string item = "item" + std::to_string(i);
        bloom.insert(item);
        hashSet.insert(item);
    }

    // Test false positive rate with items not in the set
    int falsePositives = 0;
    const int numTests = 10000;
    for (int i = numItems; i < numItems + numTests; ++i) {
        std::string item = "item" + std::to_string(i);
        if (bloom.mightContain(item)) {
            falsePositives++;
        }
    }

    // Calculate and return results
    AnalysisResult result;
    result.bloomMemoryKB = bloom.getMemoryUsage() / 1024.0;
    result.hashSetMemoryKB = (hashSet.size() * (sizeof(std::string) + sizeof(void*))) / 1024.0;
    result.savingsPercent = (1.0 - result.bloomMemoryKB/result.hashSetMemoryKB) * 100;
    result.bitsPerItem = (double)bloom.getBitArraySize() / numItems;
    result.actualFPRate = (double)falsePositives / numTests * 100;

    return result;
}

/**
 * Prints analysis results for different configurations
 */
void printAnalysis() {
    // Test configurations
    const std::vector<int> numItemsList = {1000, 10000, 100000};
    const std::vector<double> targetFPRates = {0.01, 0.05, 0.1}; // 1%, 5%, 10%

    // Set up output formatting
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nBloom Filter Analysis:\n";
    std::cout << std::string(85, '=') << "\n";
    std::cout << std::setw(10) << "Items"
              << std::setw(12) << "Target FP%"
              << std::setw(12) << "Actual FP%"
              << std::setw(15) << "Bloom (KB)"
              << std::setw(15) << "HashSet (KB)"
              << std::setw(12) << "Savings%"
              << std::setw(12) << "Bits/Item" << "\n";
    std::cout << std::string(85, '-') << "\n";

    // Test each configuration
    for (int numItems : numItemsList) {
        for (double targetFPRate : targetFPRates) {
            auto result = analyzeConfiguration(numItems, targetFPRate);

            // Print results
            std::cout << std::setw(10) << numItems
                      << std::setw(12) << targetFPRate * 100
                      << std::setw(12) << result.actualFPRate
                      << std::setw(15) << result.bloomMemoryKB
                      << std::setw(15) << result.hashSetMemoryKB
                      << std::setw(12) << result.savingsPercent
                      << std::setw(12) << result.bitsPerItem << "\n";
        }
        std::cout << std::string(85, '-') << "\n";
    }
}

int main() {
    printAnalysis();
    return 0;
}