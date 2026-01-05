#include "vm/VirtualMemoryManager.h"
#include <iostream>

VirtualMemoryManager::VirtualMemoryManager(
    MemoryManager& mm,
    Cache& l1,
    size_t total_memory,
    const std::string& policy)
    : phys_mem(mm),
      cache(l1),
      timestamp(0),
      replacement_policy(policy),
      page_faults(0),
      page_evictions(0) {

    max_frames = total_memory / PAGE_SIZE;
    used_frames = 0;
}

size_t VirtualMemoryManager::allocate_frame() {
    int id = phys_mem.allocate_first_fit(PAGE_SIZE);

    if (id == -1) {
        return static_cast<size_t>(-1);
    }

    used_frames++;
    return id;
}

size_t VirtualMemoryManager::evict_page() {
    size_t victim_vpn;

    if (replacement_policy == "FIFO") {
        victim_vpn = fifo_queue.front();
        fifo_queue.pop();
    } else { 
        size_t oldest_time = SIZE_MAX;
        size_t victim_vpn = 0;

        for (auto& entry : page_table) {
            size_t vpn = entry.first;
            PageTableEntry& pte = entry.second;

            if (pte.valid && pte.last_used < oldest_time) {
                oldest_time = pte.last_used;
                victim_vpn = vpn;
            }
        }
    }

    PageTableEntry& victim = page_table[victim_vpn];
    size_t frame = victim.frame;

    phys_mem.free_block(frame);
    victim.valid = false;
    used_frames--;
    page_evictions--;

    return frame;
}

void VirtualMemoryManager::access(size_t virtual_address) {
    timestamp++;

    size_t vpn = virtual_address / PAGE_SIZE;
    size_t offset = virtual_address % PAGE_SIZE;

    auto& pte = page_table[vpn];

    
    if (pte.valid) {
        pte.last_used = timestamp;

        size_t physical_address =
            pte.frame * PAGE_SIZE + offset;

        cache.access(physical_address);
        return;
    }

    page_faults++;
    std::cout << "[PAGE FAULT] Virtual page " << vpn << "\n";

    size_t frame;

    if (used_frames < max_frames) {
        frame = allocate_frame();
    } else {
        frame = evict_page();
    }

    
    pte.frame = frame;
    pte.valid = true;
    pte.loaded_at = timestamp;
    pte.last_used = timestamp;

    if (replacement_policy == "FIFO") {
        fifo_queue.push(vpn);
    }

    size_t physical_address =
        frame * PAGE_SIZE + offset;

    cache.access(physical_address);
}

void VirtualMemoryManager::print_stats() const {
    std::cout << "--- Virtual Memory Stats ---\n";
    std::cout << "Page faults: " << page_faults << "\n";
    std::cout << "Page evictions: " << page_evictions << "\n";
    std::cout << "Resident pages: " << used_frames << "\n";
}
