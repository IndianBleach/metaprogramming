#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

#define ALLOCED_CHUNKS_LEN 1024

#define HEAP_INIT_CAP 1024 * 64
#define HEAP_MAX_CAP 1024 * 86

#define HEAP_INIT if (!heap.initialized) { \
    MHeapInit(); \
}

typedef struct  {
    size_t size;
    bool is_freed;
    void* start_at;
} HeapChunk;

// the heap
typedef struct {
    void* start_at;
    void* curr;

    bool initialized;
    size_t max_cap;
    size_t allocated_cap;
    HeapChunk alloced_chunks[ALLOCED_CHUNKS_LEN];
    int _alloced_index;
} Mheap;

Mheap heap;


void MHeapInit() {
    void* start = (void*)HeapCreate(HEAP_NO_SERIALIZE, HEAP_INIT_CAP, HEAP_MAX_CAP);

    heap.max_cap = HEAP_MAX_CAP;
    heap.allocated_cap = 0; 
    heap.start_at = start;
    heap.initialized = true;

    heap._alloced_index = 0;
    heap.curr = start;
};

void _malloc(size_t size) {
    HEAP_INIT;
}

// двоичный поиск на основе адреса(размера) среди alloced_chunks
int _indexOfAddr(void* target, int startIndex, int endIndex) {
    //
    int mid = startIndex - endIndex;
    if (heap.alloced_chunks[mid].start_at == target) {
        return mid;
    }

    return mid;
    //if (heap.allocated_cap)
}




void _free(void* addr) {
    HEAP_INIT;



}

// dump_chunks
// dump_heap

void _heapAlloc(size_t size) {
    heap.allocated_cap += size;
    heap.curr += (int)size;
    heap._alloced_index++;
}



// allocate and push new chunk to end of heap
void* push_back(size_t size) {
    int index = heap._alloced_index;
    void* start = heap.curr; 

    // todo: add log, Asserts
    _heapAlloc(size);

    HeapChunk chunk = {.is_freed = false, .size = size, .start_at = start};
    heap.alloced_chunks[index] = chunk;

    return chunk.start_at;
}

// shift_on
/* Free()
    ИЛИ dic<index, addr> и хеш таблица 
    qsort(addr) - быстрая сортировка по alloced_chunks на основе адреса
*/ 


void dump_heap() {
    printf("dump: MHEAP\nalloced_cap+index: %i %i\nstart_at: %p\ncurr: %p\n----------------------\n", 
        heap.allocated_cap,
        heap._alloced_index,
        heap.start_at,
        heap.curr);
}


int main() { 

    _malloc(4);
   
    push_back(4);

    dump_heap();

    void* p1 = push_back(18);

    dump_heap();

    printf("index: %i \n", _getChunkIndex(p1));

    return 0;
}