# Design Document – Memory Management Simulator

## 1. Memory Layout and Assumptions

The simulator models physical memory as a **single contiguous region** of byte-addressable memory starting at address `0`. The total size of physical memory is configurable at runtime through the command-line interface.

### Assumptions
- Physical memory is contiguous and linear.
- Memory is byte-addressable.
- The simulator operates entirely in user space and does not interact with real hardware.
- All addresses used in the cache and physical memory layers are **physical addresses**.
- Virtual memory is simulated on top of this physical memory using paging.

These assumptions simplify the implementation while preserving the essential behavior of an operating system’s memory subsystem.

---

## 2. Allocation Strategy Implementations

Dynamic memory allocation is implemented using a **linked list of memory blocks**, where each block contains:
- Start address
- Size
- Allocation status (free or allocated)
- Unique block ID
- Pointer to the next block

The following allocation strategies are supported:

### First Fit
The allocator scans the memory block list from the beginning and selects the **first free block** that is large enough to satisfy the request.  
This strategy is fast but can lead to external fragmentation near the start of memory.

### Best Fit
The allocator scans the entire memory block list and selects the **smallest free block** that can satisfy the request.  
This reduces immediate wasted space but increases allocation time due to full traversal.

### Worst Fit
The allocator selects the **largest available free block**, attempting to leave larger free regions after allocation.  
This can delay fragmentation in some cases but may increase fragmentation over time.

### Splitting and Coalescing
- **Splitting:** If a free block is larger than the requested size, it is split into an allocated block and a smaller free block.
- **Coalescing:** When a block is freed, adjacent free blocks are merged to reduce external fragmentation.


---

## 3. Cache Hierarchy and Replacement Policy

### Cache Hierarchy
The simulator models a **multilevel CPU cache hierarchy** consisting of:
- L1 Cache
- L2 Cache

Each cache level is configurable in terms of:
- Cache size
- Block size
- Associativity
- Replacement policy

Caches are connected hierarchically such that a miss in L1 is forwarded to L2, and a miss in L2 is forwarded to main memory.

---

### Cache Organization
Caches are implemented as **set-associative caches**. Each cache is divided into sets, and each set contains multiple cache lines (ways).

Each cache line stores:
- Tag
- Valid bit
- Replacement metadata (timestamps or order)

Physical addresses are divided into:
- Block offset
- Set index
- Tag

---

### Replacement Policies
The following replacement policies are implemented:
- **FIFO (First-In, First-Out):** Evicts the oldest cache line in a set.
- **LRU (Least Recently Used):** Evicts the least recently accessed cache line.

Replacement decisions are made independently for each cache set.

---

### Cache Timing and Miss Penalty
Each cache level uses a **symbolic timing model**, consisting of:
- Hit time
- Miss penalty

Cache misses propagate to lower cache levels. Each level accounts for its own timing contribution, allowing **miss penalty propagation** through the hierarchy.

The simulator computes **Average Memory Access Time (AMAT)** as:

AMAT = Total Cycles / Total Accesses


---

## 4. Virtual Memory Model

The simulator implements **paging-based virtual memory**.

### Paging Parameters
- Fixed page size: 256 bytes
- Physical memory is divided into page-sized frames
- Virtual addresses are split into:
  - Virtual Page Number (VPN)
  - Offset

---

### Page Tables
Virtual-to-physical mappings are stored in a **page table**, implemented as a sparse map.

Each page table entry contains:
- Allocator block ID (representing the physical frame)
- Valid bit
- Metadata for replacement (FIFO or LRU)

Each page table entry corresponds to exactly one virtual page.

---

### Page Replacement Policies
The following page replacement policies are supported:
- FIFO
- LRU

When physical memory frames are exhausted, a victim page is selected according to the chosen policy and evicted.

The simulator tracks:
- Page faults
- Page evictions
- Number of resident pages

Disk I/O is not simulated; eviction is symbolic.

---

## 5. Address Translation Flow

All memory accesses follow the sequence:

Virtual Address
→ Page Table Lookup
→ Physical Address
→ Cache Hierarchy (L1 → L2)
→ Physical Memory


Key properties:
- Address translation is performed **before cache access**
- Caches operate strictly on **physical addresses**
- Virtual memory and caching are cleanly separated layers

This flow mirrors the behavior of real hardware memory systems.

---

## 6. Limitations and Simplifications

The simulator intentionally includes the following limitations:

- Single-process virtual memory model
- No disk or swap space simulation
- Symbolic timing instead of real hardware cycles
- No Translation Lookaside Buffer (TLB)
- No concurrent allocations or multithreading

These simplifications allow the simulator to focus on **core OS memory-management concepts** without unnecessary complexity.

---

## 7. Summary

This project implements a modular and realistic simulation of an operating system’s memory subsystem, including:
- Dynamic memory allocation with multiple strategies
- Fragmentation tracking and statistics
- Multilevel cache hierarchy with replacement policies
- Paging-based virtual memory with address translation
- Clean integration between all components

The simulator provides a clear platform for studying memory-management behavior and trade-offs in operating systems.