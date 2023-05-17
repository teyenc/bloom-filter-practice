# Bloom Filter Implementation

A space-efficient probabilistic data structure in C++ that uses multiple hash functions to test set membership - trading perfect accuracy for massive memory savings (96%+ compared to hash sets). It can tell you if an element is definitely not in a set or probably in a set, maintaining controllable false positive rates.
## What is a Bloom Filter?
A Bloom filter is a space-efficient probabilistic data structure

It quickly tells you if an element is definitely not in a set or probably in a set.
It may yield false positives (saying an element is present when it's not) but never false negatives.

## Why Bloom Filter?
For example, when you sign up Gmail, Google can use bloom filter, to know if an account name already exist without query the entire DB in O(1). But why don't we use hash set? Short answer - hash set is too big. The good thing about Bloom filter is to sacrifice a bit certainty to gain speed and space (which is super cool).

## Performance Analysis

Memory usage and accuracy comparison between Bloom Filter and HashSet:

```
Bloom Filter Analysis (1,000 items):
---------------------------------------------------------------
Target FP Rate | Actual FP Rate | Memory Savings | Bits per Item
---------------------------------------------------------------
1%            | 1.02%          | 96.25%         | 9.59
5%            | 5.29%          | 97.56%         | 6.24
10%           | 10.75%         | 98.12%         | 4.79

Results scale linearly with data size (tested up to 100,000 items)
```

Key findings:
- Achieves >96% memory savings across all configurations
- Actual false positive rates closely match target rates
- Memory efficiency increases with larger datasets
- Configurable trade-off between accuracy and memory usage

## Features

- Configurable false positive rate
- Memory-efficient bit array implementation
- Multiple hash function support
- Optimal parameter calculation based on expected items and desired false positive rate
- Memory usage tracking and reporting

## Requirements

- C++17 or later
- CMake 3.10 or later

## Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Example

```cpp
// Create a Bloom filter with optimal size for 1000 items and 1% false positive rate
auto [size, numHashes] = BloomFilter::optimalParameters(1000, 0.01);
BloomFilter bloom(size, numHashes);

// Insert items
bloom.insert("item1");
bloom.insert("item2");

// Query items
if (bloom.mightContain("item1")) {
    // Item is probably in the set
}
```

## Project Structure

```
.
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Main implementation and benchmarks
└── README.md              # This file
```

## Implementation Details

The Bloom filter uses:
- Bit-array based storage for memory efficiency
- MurmurHash-inspired hashing for uniform distribution
- Optimal sizing based on desired false positive rate
- Memory-aligned data structures