#include <stddef.h>
#include <stdbool.h>

#define ALLOCED_CHUNKS_LEN 1024

#define HEAP_INIT_CAP 1024 * 64
#define HEAP_MAX_CAP 1024 * 86

typedef struct  {
    size_t size;
    bool is_freed;
    uintptr_t* start_at;
} HeapChunk;

// the heap
typedef struct {
    uintptr_t* start_at;
    uintptr_t* curr;

    bool initialized;
    size_t max_cap;
    size_t allocated_cap;
    HeapChunk alloced_chunks[ALLOCED_CHUNKS_LEN];
    int alloced_index;
} Mheap;

void MHeapInit();

void* _malloc(size_t size);

void _free(void* addr);

void dump_chunks();

void dump_heap_array();