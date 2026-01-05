#ifndef PAGE_TABLE_ENTRY_H
#define PAGE_TABLE_ENTRY_H

#include <cstddef>

struct PageTableEntry {
    size_t frame;     
    bool valid;          
    size_t loaded_at;    
    size_t last_used;   

    PageTableEntry()
        : frame(0), valid(false),
          loaded_at(0), last_used(0) {}
};

#endif
