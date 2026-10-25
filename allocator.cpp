#include <cstddef>
#include <iostream>
#include <limits>
#include <new>

struct Block {
    std::size_t size;  // Total size of this block, including the header.
    Block* next;
};

// The payload begins immediately after the header, so keep the header size
// compatible with the maximum alignment used by this educational allocator.
static_assert(sizeof(Block) % alignof(std::max_align_t) == 0,
              "Block header size must preserve max_align_t alignment");

constexpr std::size_t kArenaSize = 4096;

std::size_t align_up(std::size_t value, std::size_t alignment) {
    if (value > std::numeric_limits<std::size_t>::max() - (alignment - 1)) {
        return 0;  // Overflow would make the request invalid.
    }
    return ((value + alignment - 1) / alignment) * alignment;
}

void* allocate(Block*& free_head, std::size_t requested_size) {
    if (free_head == nullptr || requested_size == 0) {
        return nullptr;
    }

    constexpr std::size_t alignment = alignof(std::max_align_t);
    const std::size_t payload_size = align_up(requested_size, alignment);
    if (payload_size == 0 ||
        payload_size > std::numeric_limits<std::size_t>::max() - sizeof(Block)) {
        return nullptr;
    }
    const std::size_t required_size = sizeof(Block) + payload_size;

    // First-fit search over the address-ordered free list.
    Block* previous = nullptr;
    Block* current = free_head;
    while (current != nullptr && current->size < required_size) {
        previous = current;
        current = current->next;
    }
    if (current == nullptr) {
        return nullptr;
    }

    const std::size_t remainder_size = current->size - required_size;
    Block* const next_free = current->next;

    // Split only if the remainder can hold a header and an aligned payload.
    if (remainder_size >= sizeof(Block) + alignment) {
        auto* remainder_address = reinterpret_cast<std::byte*>(current) + required_size;
        Block* remainder = new (remainder_address) Block{remainder_size, next_free};
        if (previous != nullptr) {
            previous->next = remainder;
        } else {
            free_head = remainder;
        }
        current->size = required_size;
    } else {
        if (previous != nullptr) {
            previous->next = next_free;
        } else {
            free_head = next_free;
        }
    }

    current->next = nullptr;
    return reinterpret_cast<std::byte*>(current) + sizeof(Block);
}

// Precondition: pointer is null or was returned by allocate() and has not
// already been deallocated. Invalid and duplicate frees are not detected.
void deallocate(Block*& free_head, void* pointer) {
    if (pointer == nullptr) {
        return;
    }

    auto* current = reinterpret_cast<Block*>(
        reinterpret_cast<std::byte*>(pointer) - sizeof(Block));

    // Insert by address so adjacent blocks can be found during coalescing.
    Block* previous = nullptr;
    Block* next = free_head;
    auto* current_address = reinterpret_cast<std::byte*>(current);
    while (next != nullptr &&
           reinterpret_cast<std::byte*>(next) < current_address) {
        previous = next;
        next = next->next;
    }

    current->next = next;
    if (previous != nullptr) {
        previous->next = current;
    } else {
        free_head = current;
    }
}

void coalesce(Block* free_head) {
    Block* current = free_head;
    while (current != nullptr && current->next != nullptr) {
        Block* next = current->next;
        auto* end_of_current = reinterpret_cast<std::byte*>(current) + current->size;
        if (end_of_current == reinterpret_cast<std::byte*>(next)) {
            current->size += next->size;
            current->next = next->next;
            // Stay on current: it may now be adjacent to another free block.
        } else {
            current = next;
        }
    }
}

int main() {
    alignas(std::max_align_t) std::byte arena[kArenaSize];
    Block* free_head = new (arena) Block{kArenaSize, nullptr};

    void* first = allocate(free_head, 128);
    void* second = allocate(free_head, 256);
    if (first == nullptr || second == nullptr) {
        std::cerr << "Allocation failed\n";
        return 1;
    }
    std::cout << "Allocated two blocks from the 4 KiB arena.\n";

    deallocate(free_head, first);
    deallocate(free_head, second);
    coalesce(free_head);

    void* reused = allocate(free_head, 512);
    if (reused == nullptr) {
        std::cerr << "Allocation after coalescing failed\n";
        return 1;
    }
    std::cout << "Allocation after freeing and coalescing succeeded.\n";

    deallocate(free_head, reused);
    coalesce(free_head);
    std::cout << "Demo complete.\n";
    return 0;
}
