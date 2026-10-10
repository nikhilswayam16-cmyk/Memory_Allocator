#include<iostream>
#include<cstddef>
#include<new>
using namespace std;
struct block{
size_t size;
block* next;
};
int main()
{
alignas(block) byte arena[4096];
block* A = new (arena) block {4096,nullptr};
block* free_hand = A;
cout<<sizeof(arena);
cout<<'\n';
cout<<free_hand->size<<'\n';
cout<<sizeof(block)<<'\n';
if(free_hand->next == nullptr) cout<<1<<'\n';
return 0;
}
