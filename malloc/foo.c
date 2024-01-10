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
    int alloced_index;
} Mheap;

Mheap heap;



void MHeapInit() {
    void* start = (void*)HeapCreate(HEAP_NO_SERIALIZE, HEAP_INIT_CAP, HEAP_MAX_CAP);

    heap.max_cap = HEAP_MAX_CAP;
    heap.allocated_cap = 0; 
    heap.start_at = start;
    heap.initialized = true;

    heap.alloced_index = 0;
    heap.curr = start;
};


void _heapAlloc(size_t size) {
    heap.alloced_index += 1;
    heap.allocated_cap += size;
    heap.curr += (int)size;
}

// allocate and push new chunk to end of heap
void* push_back(size_t size) {
    int index = heap.alloced_index;
    void* start = heap.curr; 

    // todo: add log, Asserts
    _heapAlloc(size);

    HeapChunk chunk = {.is_freed = false, .size = size, .start_at = start};
    heap.alloced_chunks[index] = chunk;

    return chunk.start_at;
}

// [+] двоичный поиск на основе адреса(размера) среди alloced_chunks
int _indexOfAddr(void* target, int startIndex, int endIndex) {
    
    if (startIndex > endIndex) return -1;

    int equal_flag = 0;

    int mid =  (endIndex - startIndex) / 2;

    while(startIndex <= endIndex) {
        
        mid =  (endIndex - startIndex) / 2 + startIndex;
        
        if (endIndex - startIndex == 1) {
            mid = startIndex;
        }

        if (heap.alloced_chunks[mid].start_at == target) {
            return mid;
        }
        else if (heap.alloced_chunks[mid].start_at < target) {
            startIndex = mid + 1;
            if (mid == heap.alloced_index - 1) return -1;
        }
        else if (heap.alloced_chunks[mid].start_at > target) {
            endIndex = mid - 1;
            if (mid == 0) return -1;
        }
    }

    return -1;  
}

// [+] поиск среди фришных - комбинация бинпоиск + проход
int _indexOfMinFreed(size_t target, int startIndex, int endIndex) {

    int mid = (endIndex - startIndex) / 2;

    while(startIndex <= endIndex) {

        mid = (endIndex - startIndex) / 2;

        if (heap.alloced_chunks[mid].size > target) {
            endIndex = mid--;
        }
        else if (heap.alloced_chunks[mid].size < target) {
            startIndex = mid++;
        }
        else startIndex--;


        if (mid <= 1)break;

        if (mid == heap.alloced_index - 1) {
            return -1;
        }
    }

    if (mid <= 1) mid = 0;

    while(mid < heap.alloced_index) {
        //printf("arr[%i] free=%i size=%i \n", mid, heap.alloced_chunks[mid].is_freed, heap.alloced_chunks[mid].size);
        if (heap.alloced_chunks[mid].is_freed &&
            heap.alloced_chunks[mid].size >= target) {
            return mid;
        }
        mid++;
    }

    return -1;
}

void dump_heap() {
    printf("dump: MHEAP\nalloced_cap+index: %i %i\nstart_at: %p\ncurr: %p\n----------------------\n", 
        heap.allocated_cap,
        heap.alloced_index,
        heap.start_at,
        heap.curr);
}

void dump_chunks() {
    printf("dump: ALLOCED_CHUNKS\ncount: %i\n", heap.alloced_index);
    for(int i = 0; i < heap.alloced_index; i++) {
        printf("start: %p\nsize: %i\nfree: %i\n------------\n", heap.alloced_chunks[i].start_at, heap.alloced_chunks[i].size, heap.alloced_chunks[i].is_freed);
    }
}

void dump_heap_info() {
    printf("dump: HEAP\n alloced= %i, last->free=%i\n===================\n", heap.alloced_index, heap.alloced_chunks[heap.alloced_index-1].is_freed);
}

void dump_heap_array() {
    printf("heap [");
    for(int i = 0; i < heap.alloced_index; i++) {
        if (i == heap.alloced_index -1) {
            printf("%i(%i) ]\n", heap.alloced_chunks[i].size, heap.alloced_chunks[i].is_freed);
        }
        else printf("%i(%i), ", heap.alloced_chunks[i].size, heap.alloced_chunks[i].is_freed);
    }
}



void* _malloc(size_t size) {
    
    if (size <= 0) return NULL; 

    HEAP_INIT;

    int freed_index = _indexOfMinFreed(size, 0, heap.alloced_index);

    if (freed_index != -1) {
        // insert_at
        // - shift_on
        // - reorder
    }
    else {
        void* addr = (void*)push_back(size);
        return addr;
    }
}

// todo: попробовать реализовать фри через хеш таблицу


void _free(void* addr) {
    HEAP_INIT;

    // todo: asserts, if

    int findIndex = _indexOfAddr(addr, 0, heap.alloced_index);

    heap.alloced_chunks[findIndex].is_freed = true;
    heap.allocated_cap -= heap.alloced_chunks[findIndex].size;

    //dump_chunks();
    //*(uintptr_t*)addr = 0;
}


// склеивает все непрерывные чанки в один
void heap_collect() {

}

void heap_reorder() {
    
}

void heap_shift_right(int startIndex) {
    // todo: asserts
    if (startIndex < ALLOCED_CHUNKS_LEN) {
        
        for(int i = heap.alloced_index-1; i >= startIndex; i--) {
            heap.alloced_chunks[i+1] = heap.alloced_chunks[i];
        }

        heap.alloced_chunks[startIndex].size = 0; 

        heap.alloced_index += 1;
    }
}

// инсерт в чанк, разделяет его на две новые части
void* insert_at(int index, size_t target_size) {
    // update cur
    // shift

    if (index >= heap.alloced_index) {
        return NULL;
    }

    size_t free_size = heap.alloced_chunks[index].size - target_size;
    int new_index = index + 1;

    printf("st=%p\n", heap.alloced_chunks[index].start_at);
    void* new_addr = heap.alloced_chunks[index].start_at + (int)target_size;

    printf(" size=%i\nleft: %p\nright:%p\n", target_size, heap.alloced_chunks[index].start_at, new_addr);

    // update cur chunk 
    heap.alloced_chunks[index].size = target_size;
    heap.alloced_chunks[index].is_freed = false;

    dump_heap_array();

    if (free_size > 0) {
        heap_shift_right(new_index);
        HeapChunk right_chunk = {.is_freed = true, .size = free_size, .start_at = new_addr};
        heap.alloced_chunks[new_index] = right_chunk;
    }

    dump_heap_array();

    // create new chunk

    //heap.alloced_index++;
    // todo: инкремент alloced_index to func
}




// shift_on
/* Free()
    ИЛИ dic<index, addr> и хеш таблица 
    qsort(addr) - быстрая сортировка по alloced_chunks на основе адреса
*/ 


// heap_collect
// reorder


int main() { 

    //_malloc(6);
   
    //dump_chunks();

    int* p1 = (int*)_malloc(1);

    int* p2 = (int*)_malloc(2);

    int* p3 = (int*)_malloc(3);

    _free(p3);

    dump_heap_array();

    // [1(0), 2(1), 3(0) ]
    // insert_at(1, 1);
    // [1(0), 1(0), 1(1), 3(0) ]

    insert_at(2, 1);

    dump_heap_array();

    dump_chunks();

    //printf("res=%i \n", _indexOfAddr((void*)p1, 0, heap.alloced_index));

    //_indexOfAddr


    return 0;
}