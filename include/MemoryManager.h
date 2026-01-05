#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "MemoryBlock.h"

class MemoryManager {
private:
	MemoryBlock* head;
	size_t total_memory;
	int next_block_id;
	MemoryBlock* split_and_allocate(MemoryBlock* block, size_t req_size);

public:
	MemoryManager();
	~MemoryManager();

	void init(size_t size);
	void dump() const;
	int allocate_first_fit(size_t size);
	bool free_block(int block_id);
	int allocate_best_fit(size_t size);
	int allocate_worst_fit(size_t size);
	size_t total_free_memory() const;
	size_t largest_free_block() const;
	double external_fragmentation() const;
	double memory_utilization() const;
};

#endif
