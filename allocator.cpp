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
size_t req = alloted_size + sizeof(block);
block* past = nullptr;
block* present = free_hand;
while(present != nullptr && present->size < req)
{
past = present;
present = present->next;
}
if(present == nullptr) return nullptr;
size_t sr = present->size - req;
if(sr >= sizeof(block) + alignment)
{
block* remainder = new(reinterpret_cast<byte*>(present) + req) block{sr,nullptr};
present->size = req;
if(past != nullptr)
{
past->next = remainder;
remainder->next = present->next;
}
else
{
free_hand = remainder;
remainder->next = present->next;
}
return reinterpret_cast<byte*>(present) + sizeof(block);
}
else{
else {
    if (past != nullptr)
        past->next = present->next;
    else
        free_hand = present->next;

    return reinterpret_cast<byte*>(present) + sizeof(block);
}
}
}
void deallocate(block*& free_hand, void* p)
{
if(p == nullptr) return;
block* current = reinterpret_cast<block*>( reinterpret_cast<byte*>(p) - sizeof(block));
block* t = free_hand;
if(t == nullptr){
current->next = nullptr;
free_hand = current;
return;
}
if( reinterpret_cast<byte*>(current) < reinterpret_cast<byte*>(free_hand) )
{
free_hand = current;
current->next = t;
}
else{
block* here = free_hand;
block* there = free_hand->next;
byte* ta = reinterpret_cast<byte*>(there);
byte* ca = reinterpret_cast<byte*>(current);
while( there != nullptr && ta < ca)
{
here = there;
there = there->next;
ta = reinterpret_cast<byte*>(there);
}
here->next = current;
current->next = there;
}
}
void coalesce(block* free_hand)
{
if(free_hand == nullptr) return;
block* p = free_hand->next;
if(p == nullptr) return;
block* here = free_hand;
block* there = free_hand->next;
while(there != nullptr)
{
if( reinterpret_cast<byte*>(here) + here->size == reinterpret_cast<byte*>(there))
{
here->size = here->size + there->size;
here->next = there->next;
there = there->next;
}
else
{
here = here->next;
there = there->next;
}
}
}
}
int main()
{
alignas(block) byte arena[4096];
block* A = new (arena) block {4096,nullptr};
block* free_hand = A;
return 0;
}
