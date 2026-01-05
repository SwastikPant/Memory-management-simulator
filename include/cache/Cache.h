#ifndef CACHE_H
#define CACHE_H

#include "cache/CacheSet.h"
#include <vector>
#include <string>

class Cache {
private:
    size_t cache_size;
    size_t block_size;
    size_t associativity;
    size_t num_sets;

    std::string replacement_policy;

    std::vector<CacheSet> sets;

    size_t timestamp;
    size_t hits;
    size_t misses;

    Cache* next_level; 

public:
    Cache(size_t cache_size,
          size_t block_size,
          size_t associativity,
          const std::string& policy);

    void set_next_level(Cache* next);

    void access(size_t address);

    void print_stats(const std::string& name) const;
};

#endif
