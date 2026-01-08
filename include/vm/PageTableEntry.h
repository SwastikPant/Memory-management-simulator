#ifndef PAGE_TABLE_ENTRY_H
#define PAGE_TABLE_ENTRY_H

#include <cstddef>

struct PageTableEntry {
    int block_id;
    bool valid;
    size_t loaded_at;
    size_t last_used;

    PageTableEntry()
        : block_id(-1), valid(false),
          loaded_at(0), last_used(0) {}
};

#endif
