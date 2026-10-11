#include<iostream>
#include<cstddef>
#include<new>
using namespace std;
struct block{
size_t size;
block* next;
};
void* allocate(block*& free_hand, size_t requested_size)
{
if(free_hand == nullptr) return nullptr;
size_t alignment = alignof(block);
size_t alloted_size{};
if(requested_size % alignment == 0){
alloted_size = requested_size;
}
else{
size_t a = requested_size/alignment;
alloted_size = alignment * (a+1);
}
if(alloted_size + sizeof(block) > free_hand->size) return nullptr;
size_t sr = free_hand->size - alloted_size - sizeof(block);
block* remainder = new(reinterpret_cast<byte*>(free_hand) + alloted_size + sizeof(block)) block {sr, nullptr};
free_hand->size = alloted_size + sizeof(block);
block* t = free_hand;
free_hand = remainder;
return reinterpret_cast<byte*>(t) + sizeof(block);
}
void deallocate(block*& free_hand, void* p)
{ 
block* current = reinterpret_cast<block*>( reinterpret_cast<byte*>(p) - sizeof(block));
block* t = free_hand;
free_hand = current;
current->next = t;
}
void coalesce(block* free_hand)
{
if(free_hand == nullptr) return;
block* p = free_hand->next;
if(p == nullptr) return;
if(reinterpret_cast<byte*>(free_hand) + free_hand->size == reinterpret_cast<byte*>(free_hand->next)){
free_hand->size = free_hand->size + free_hand->next->size;
free_hand->next = p->next;
}
}
int main()
{
alignas(block) byte arena[4096];
block* A = new (arena) block {4096,nullptr};
block* free_hand = A;
return 0;
}
