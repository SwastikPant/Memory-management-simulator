#include "MemoryManager.h"
#include <iostream>
#include <iomanip>

MemoryManager::MemoryManager()
    : head(nullptr), total_memory(0), next_block_id(1) {}

MemoryManager::~MemoryManager() {
    MemoryBlock* curr = head;
    while (curr) {
        MemoryBlock* next = curr->next;
        delete curr;
        curr = next;
    }
}

void MemoryManager::init(size_t size) {
    total_memory = size;
    head = new MemoryBlock(0, size);
}

void MemoryManager::dump() const {
    MemoryBlock* curr = head;
    while (curr) {
        std::cout << "[0x" << std::hex << std::setw(4) << std::setfill('0') << curr->start << " - 0x" 
                  << std::setw(4) << (curr->start + curr->size - 1) << "] ";

        if (curr->free)
            std::cout << "FREE\n";
        else
            std::cout << "USED (id=" << curr->block_id << ")\n";

        curr = curr->next;
    }


    std::cout << "\n--- Metrics ---\n";
    std::cout << "Total free memory: " << total_free_memory() << "\n";
    std::cout << "Largest free block: " << largest_free_block() << "\n";
    std::cout << "External fragmentation: " << external_fragmentation() << "\n";
    std::cout << "Memory utilization: " << memory_utilization() << "\n";
}

MemoryBlock* MemoryManager::split_and_allocate(
    MemoryBlock* block, size_t req_size) {

    // Exact fit
    if (block->size == req_size) {
        block->free = false;
        block->block_id = next_block_id++;
        return block;
    }

    // Split block
    MemoryBlock* new_block =
        new MemoryBlock(block->start + req_size,
                        block->size - req_size);

    new_block->next = block->next;
    new_block->prev = block;

    if (block->next)
        block->next->prev = new_block;

    block->next = new_block;

    block->size = req_size;
    block->free = false;
    block->block_id = next_block_id++;

    return block;
}

int MemoryManager::allocate_first_fit(size_t req_size) {
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free && curr->size >= req_size) {
            return split_and_allocate(curr, req_size)->block_id;
        }
        curr = curr->next;
    }

    return -1;
}

int MemoryManager::allocate_best_fit(size_t req_size) {
    MemoryBlock* curr = head;
    MemoryBlock* best = nullptr;
    size_t best_diff = SIZE_MAX;

    while (curr) {
        if (curr->free && curr->size >= req_size) {
            size_t diff = curr->size - req_size;
            if (diff < best_diff) {
                best_diff = diff;
                best = curr;
            }
        }
        curr = curr->next;
    }

    if (!best)
        return -1;

    return split_and_allocate(best, req_size)->block_id;
}

int MemoryManager::allocate_worst_fit(size_t req_size) {
    MemoryBlock* curr = head;
    MemoryBlock* worst = nullptr;
    size_t worst_size = 0;

    while (curr) {
        if (curr->free && curr->size >= req_size) {
            if (curr->size > worst_size) {
                worst_size = curr->size;
                worst = curr;
            }
        }
        curr = curr->next;
    }

    if (!worst)
        return -1;

    return split_and_allocate(worst, req_size)->block_id;
}



bool MemoryManager::free_block(int block_id) {
    MemoryBlock* curr = head;
    while (curr) {
        if (!curr->free && curr->block_id == block_id) {

            // Mark block as free
            curr->free = true;
            curr->block_id = -1;

            // Combine with next block if free
            if (curr->next && curr->next->free) {
                MemoryBlock* next = curr->next;
                curr->size += next->size;
                curr->next = next->next;
                if (next->next)
                    next->next->prev = curr;
                delete next;
            }

            // Combine with previous block if free
            if (curr->prev && curr->prev->free) {
                MemoryBlock* prev = curr->prev;
                prev->size += curr->size;
                prev->next = curr->next;
                if (curr->next)
                    curr->next->prev = prev;
                delete curr;
            }

            return true;
        }

        curr = curr->next;
    }

    return false; // block ID not found
}


size_t MemoryManager::total_free_memory() const {
    size_t total = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free)
            total += curr->size;
        curr = curr->next;
    }

    return total;
}

size_t MemoryManager::largest_free_block() const {
    size_t largest = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free && curr->size > largest)
            largest = curr->size;
        curr = curr->next;
    }

    return largest;
}

double MemoryManager::external_fragmentation() const {
    size_t total_free = total_free_memory();

    if (total_free == 0)
        return 0.0;

    size_t largest = largest_free_block();
    return 1.0 - (double)largest / total_free;
}

double MemoryManager::memory_utilization() const {
    size_t used = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (!curr->free)
            used += curr->size;
        curr = curr->next;
    }

    return (double)used / total_memory;
}

