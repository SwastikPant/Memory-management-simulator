#include "cache/Cache.h"
#include <iostream>

Cache::Cache(size_t csize,
             size_t bsize,
             size_t assoc,
             const std::string& policy)
    : cache_size(csize),
      block_size(bsize),
      associativity(assoc),
      replacement_policy(policy),
      timestamp(0),
      hits(0),
      misses(0),
      hit_time(1),
      miss_penalty(10),
      total_accesses(0),
      total_cycles(0),
      next_level(nullptr) {

    num_sets = cache_size / (block_size * associativity);

    for (size_t i = 0; i < num_sets; ++i) {
        sets.emplace_back(associativity);
    }
}


void Cache::set_next_level(Cache* next) {
    next_level = next;
}

void Cache::access(size_t address) {
    timestamp++;
    total_accesses++;

    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    bool hit = false;
    sets[set_index].access(tag, replacement_policy, timestamp, hit);

    if (hit) {
        hits++;
        total_cycles += hit_time;
    } else {
        misses++;
        total_cycles += hit_time  + miss_penalty;
        if (next_level) {
            next_level->access(address);
        }
    }
}

double Cache::amat() const {
    if (total_accesses == 0) return 0.0;
    return (double)total_cycles / total_accesses;
}


void Cache::print_stats(const std::string& name) const {
    std::cout << "--- " << name << " Cache Stats ---\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";

    size_t total = hits + misses;
    if (total > 0) {
        std::cout << "Hit rate: "
                  << (double)hits / total << "\n";
    }

    std::cout << "Average Memory Access Time: " << amat() << " cycles\n";
}
