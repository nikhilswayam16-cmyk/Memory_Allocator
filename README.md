# Memory Allocator in C++

A small educational memory allocator that manages a fixed 4 KiB byte arena. It demonstrates how a simple allocator can track free memory with block headers and a linked free list instead of asking the operating system for memory on every allocation.

## Features

- **Fixed arena:** starts with one free block covering a preallocated 4096-byte buffer.
- **Block metadata:** each block stores its total size and a pointer to the next free block.
- **First-fit allocation:** searches the address-ordered free list for the first block large enough for a request.
- **Alignment:** rounds payload sizes to `alignof(std::max_align_t)` and returns an appropriately aligned payload for the supported layout.
- **Block splitting:** divides a larger free block when the leftover region can hold a header and an aligned payload.
- **Deallocation:** recovers the block header from the payload pointer and inserts the block into the free list in address order.
- **Coalescing:** joins adjacent free blocks to reduce fragmentation.
- **Placement new:** constructs block metadata directly in the arena.

## Build and run

Requires a C++17-compatible compiler such as `g++`.

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic allocator.cpp -o allocator
./allocator
```

Expected output:

```text
Allocated two blocks from the 4 KiB arena.
Allocation after freeing and coalescing succeeded.
Demo complete.
```

## Design overview

1. The arena begins as one free block. The block's `size` includes its header as well as its payload region.
2. `allocate()` aligns the request, searches the free list, and either splits a sufficiently large block or removes the whole block from the free list.
3. `deallocate()` locates the header immediately before the payload and inserts the returned block into the free list by address.
4. `coalesce()` checks neighboring free-list entries and merges blocks whose memory ranges touch.

The small demo in `main()` allocates two blocks, frees them, coalesces the free list, and then verifies that a larger allocation can succeed.

## Limitations

This is a learning project, not a general-purpose replacement for `malloc`, `new`, or the standard allocator library.

- Capacity is limited to the fixed 4 KiB arena.
- The caller must pass only pointers returned by `allocate()` to `deallocate()` and must not free a pointer twice; invalid and duplicate frees are not detected.
- The allocator is not thread-safe.
- It does not implement arena growth, per-allocation diagnostics, or a full test suite.

## Repository

Source: [`allocator.cpp`](allocator.cpp)
