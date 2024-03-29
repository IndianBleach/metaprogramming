#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

#include "_malloc.h" 

#define HEAP_INIT if (!heap.initialized) { \
    MHeapInit(); \
}

Mheap heap;

void MHeapInit() {
    uintptr_t* start = (uintptr_t*)HeapCreate(HEAP_NO_SERIALIZE, HEAP_INIT_CAP, HEAP_MAX_CAP);
    printf("start=%p\n", start);
    //printf("start=%p\n", *start);
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
uintptr_t* push_back(size_t size) {
    int index = heap.alloced_index;
    uintptr_t* start = heap.curr; 

    // todo: add log, Asserts
    _heapAlloc(size);

    HeapChunk chunk = {.is_freed = false, .size = size, .start_at = start};
    heap.alloced_chunks[index] = chunk;

    return chunk.start_at;
}

// [+] двоичный поиск на основе адреса(размера) среди alloced_chunks
int _heap_indexOfChunk(uintptr_t* target, int startIndex, int endIndex) {
    
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

int _heap_indexOfChunkNoFreed(uintptr_t* target, int startIndex, int endIndex) {
    
    if (startIndex > endIndex) return -1;

    int equal_flag = 0;

    int mid =  (endIndex - startIndex) / 2;

    while(startIndex <= endIndex) {
        
        mid =  (endIndex - startIndex) / 2 + startIndex;
        
        if (endIndex - startIndex == 1) {
            mid = startIndex;
        }

        if (heap.alloced_chunks[mid].start_at == target && heap.alloced_chunks[mid].is_freed == false) {
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

    int prev = -1;

    while(startIndex <= endIndex) {

        mid = (endIndex - startIndex) / 2;

        if (prev == mid ||
            prev - mid == -1) {
                mid = prev;
                break;
            }

        prev = mid;

        //printf("mid=%i\n", mid);

        if (heap.alloced_chunks[mid].size > target) {
            endIndex = mid--;
            //
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

void heap_shift_left(int startIndex, int toStep) {

    if (startIndex < 0) return;

    if (startIndex == heap.alloced_index-1) {
        heap.alloced_index--;
        heap.alloced_chunks[startIndex].size = 0;
        heap.alloced_chunks[startIndex].start_at = NULL;
        heap.alloced_chunks[startIndex].is_freed = false;
        return;
    }

    for(int i = startIndex; i < heap.alloced_index-toStep; i++) {
        heap.alloced_chunks[i] = heap.alloced_chunks[i+toStep];
    }

    for(int i = heap.alloced_index-toStep; i < heap.alloced_index; i++) {
        heap.alloced_chunks[i].size = 0;
        heap.alloced_chunks[i].start_at = NULL;
        heap.alloced_chunks[i].is_freed = false;
        heap.alloced_index--;
    }
}

// инсерт в чанк, разделяет его на две новые части
uintptr_t* insert_at(int index, size_t target_size) {
    // update cur
    // shift

    if (index >= heap.alloced_index) {
        return NULL;
    }

    size_t free_size = heap.alloced_chunks[index].size - target_size;
    int new_index = index + 1;

    //printf("st=%p\n", heap.alloced_chunks[index].start_at);
    uintptr_t* new_addr = heap.alloced_chunks[index].start_at + (int)target_size;

    //printf(" size=%i\nleft: %p\nright:%p\n", target_size, heap.alloced_chunks[index].start_at, new_addr);

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

    return new_addr;

    // create new chunk

    //heap.alloced_index++;
    // todo: инкремент alloced_index to func
}

int sort_chunk_compare(const void* a, const void* b) {
    HeapChunk ca = * ((HeapChunk*)a);
    HeapChunk cb = * ((HeapChunk*)b);

    if (ca.size == cb.size) return 0;
    else if (cb.size > ca.size) return -1;
    else return 1; 

}

void* _malloc(size_t size) {
    if (size <= 0) return NULL; 

    HEAP_INIT;

    //printf("(%i) MALLOC-before: %i \n", size, heap.alloced_index);

    int freed_index = _indexOfMinFreed(size, 0, heap.alloced_index);

    if (freed_index != -1) {
        insert_at(freed_index, size);
        heap.freed_now++;
        qsort(heap.alloced_chunks, heap.alloced_index, sizeof(HeapChunk), sort_chunk_compare);
        // insert_at
        // - reorder
    }
    else {
        void* addr = (void*)push_back(size);
        return addr;
    }
}

// todo: попробовать реализовать фри через хеш таблицу

// heap_collect
// - update_cur
// - shift_left

void heap_merge_chunks(int targetStart, int end) {
    
    if (targetStart >= end) return;

    //printf("merge: %i -> %i \n", targetStart, end);

    int i = targetStart+1;
    while(i <= end) {
        heap.alloced_chunks[targetStart].size += heap.alloced_chunks[i].size;
        i++;
    }

    heap_shift_left(targetStart+1, end - targetStart);
    heap.freed_now--;
}

void heap_collect_free() {
    
    dump_heap_array();

    for(int i = 0; i < heap.alloced_index; i++) {
    
        if (heap.alloced_chunks[i].is_freed) {

            // find next freed
            int j = i;
            while(heap.alloced_chunks[j++].is_freed) 
            {}

            if ((j - i) > 1) {
                heap_merge_chunks(i, j-2);
            }
        }
    }

    qsort(heap.alloced_chunks, heap.alloced_index, sizeof(HeapChunk), sort_chunk_compare);
}

void _free(void* addr) {
    HEAP_INIT;

    // todo: asserts, if

    int findIndex = _heap_indexOfChunk((uintptr_t*)addr, 0, heap.alloced_index);

    if (findIndex != -1 && heap.alloced_chunks[findIndex].is_freed == false) {
        heap.alloced_chunks[findIndex].is_freed = true;
        heap.allocated_cap -= heap.alloced_chunks[findIndex].size;
        heap.freed_now++;
    }
}

/* Free()
    ИЛИ dic<index, addr> и хеш таблица 
    qsort(addr) - быстрая сортировка по alloced_chunks на основе адреса
*/ 

typedef struct {
    int* pt1;
} Foo;

int main2() { 

    register void* rsp __asm__("rsp");
    register void* ebp __asm__("ebp");

    int* ptr = (int*)_malloc(sizeof(int));
    int* ptr2 = (int*)_malloc(sizeof(int));

    void* cur = rsp;
    
    while(cur < ebp) {
        
        if (((uintptr_t)cur) % 8 == 0) {
            //printf("cur=%p \n", cur);
            void* pt = (void*)cur;
            printf("hp=%p ref=%p \n", pt, *(uintptr_t*)pt);
        }
        

        cur+=1;
    }    

    //void* hp = (void*)ptr;
    
    //printf("hp=%p \n", hp);

    //dump_chunks();

    printf("stack=%p, heap=%p \n", &ptr, (uintptr_t*)ptr);
    printf("stack=%p, heap=%p \n", &ptr2, (uintptr_t*)ptr2);
    //printf("void*=%p, val=%p \n", p1->pt1, *(uintptr_t*)(p1->pt1));

    //heap_shift_left(1, 1);

    // 1 2 1 2(0)

    //printf("res=%i \n", _heap_indexOfChunk((void*)p1, 0, heap.alloced_index));

    return 0;
}