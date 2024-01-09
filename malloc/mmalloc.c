#define _XOPEN_SOURCE_EXTENDED 1
#include <heapapi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

// heap mem total size
// 
// 

#define HEAP_SIZE_BIT 64000
#define HEAP_SIZE HEAP_SIZE_BIT/sizeof(char)

HANDLE hheap;
heap = HeapCreate(HEAP_NO_SERIALIZE, 1024, 0);
char heap[] = (int*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, HEAP_SIZE);

typedef struct {
    bool is_initialized; // init flag
    size_t cur_ptr; // current
    
    HANDLE h_heap;
    char heap[];

} Hheap;

Hheap* hheap_init() {
    
}

int* foo()
{



    int* heapBlock = 

    heapBlock[0] = 1;
    heapBlock[1] = 2;
    heapBlock[2] = 3;
    heapBlock[3] = 4;
    heapBlock[4] = 5;
    heapBlock[5] = 6;

    return heapBlock;
}

int main(int argc, char *argv[]){

    int* heapBlock = foo();

    printf("%p\n", &heapBlock[0]);
    printf("%p\n", &heapBlock[1]);

    for(int i = 0; i < 6; i++) {
        printf("%i \n", heapBlock[i]);
    }

    //printf("%p \n", heapBlock);

    return 0;
}