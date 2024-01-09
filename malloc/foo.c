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

// todo: попробовать реализовать фри через хеш таблицу

// двоичный поиск на основе адреса(размера) среди alloced_chunks
int _indexOfAddr(void* target, int startIndex, int endIndex) {
    
    if (startIndex > endIndex) return -1;

    while(startIndex <= endIndex) {
        int mid =  (endIndex - startIndex) / 2;
        //printf("%i-%i=%i\n", endIndex, startIndex, mid);
        


        printf("start=%i end=%i mid=%i\n", startIndex, endIndex, mid);

        if (heap.alloced_chunks[mid].start_at == target) {
            return mid;
        }
        else if (mid == 1) {
            if (heap.alloced_chunks[0].start_at == target) return 0;
            else return -1; 
        }
        else if (heap.alloced_chunks[mid].start_at < target) {
            startIndex = mid++;
            printf("fuick\n");
            //return _indexOfAddr(target, mid++, endIndex--);
        }
        else if (heap.alloced_chunks[mid].start_at < target) {
            endIndex = mid--;
            printf("fuick\n");
            //return _indexOfAddr(target, startIndex, mid--);
        }

        
    }

    return -1;  
}




void _free(void* addr) {
    HEAP_INIT;

    int findIndex = _indexOfAddr(addr, 0, heap._alloced_index);



    for(int i = 0; i < heap._alloced_index; i++) {
        if (heap.alloced_chunks[i].start_at == addr) {
            heap.alloced_chunks[i].is_freed = true;
            heap.allocated_cap -= heap.alloced_chunks[i].size;
        }
    }
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

void dump_chunks() {
    printf("dump: ALLOCED_CHUNKS\ncount: %i\n", heap._alloced_index);
    for(int i = 0; i < heap._alloced_index; i++) {
        printf("start: %p\nsize: %i\nfree: %i\n------------\n", heap.alloced_chunks[i].start_at, heap.alloced_chunks[i].size, heap.alloced_chunks[i].is_freed);
    }
}


int main() { 

    _malloc(4);
   
    void* p2 = push_back(4);

    void* p1 = push_back(18);

    _free(p1);

    dump_chunks();

    printf("index: %i \n", _indexOfAddr(p1, 0, heap._alloced_index));

    return 0;
}