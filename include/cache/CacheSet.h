#ifndef CACHE_SET_H
#define CACHE_SET_H

#include "cache/CacheLine.h"
#include <vector>
#include <string>

class CacheSet {
private:
    std::vector<CacheLine> lines;

public:
    CacheSet(size_t associativity);

    bool access(size_t tag,
                const std::string& policy,
                size_t timestamp,
                bool& hit);
};

#endif
