#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

#include "../malloc/_malloc.h"
#include <pthread.h>

#define LIST_DEFAULT_LEN 200

#define LIST_EMPTY_TREENODE 
#define LIST_EMPTY_VAL 9000000000000000

#define GC_SET_FRAME \
    register void* rsp __asm__("rsp"); \
    register void* ebp __asm__("ebp"); \
    _rsp = rsp; \
    _ebp = ebp;


//#define GC_SET_FRAME gc_updateFuncFrame();

const int STACK_FRAME_PTRDIFF = sizeof(uintptr_t);

extern int _heap_indexOfChunk(uintptr_t* target, int startIndex, int endIndex);

// todo: sortedList flexible capacity
// add gc.reserveRef(ref)
// add gc main class
// add malloc realization with heap
// add gcHeapRefTree.addRoot?

/*
    когда собирать дерево?
    как маркнуть обьекты в куче:
        предполагается что в дереве сейчас активные обьекты, остальные - нет
        -> начинаем скан в куче и сверяем с деревом

    ДЛЯ ОЧИСТКИ (С КУЧИ)
    1) начинаем бежать по куче (нужна метка на конец скана)
    2) у нас уже должны быть маркнуты root в дереве, которые могут быть очищены
    2) сверяем есть ли chunk.addr 

    gc.mark делать вне finallize?
        - будут готовые поколения обьектов gen0 (первая на очистку), gen1, gen2

    3. gc.finallize
        получить процент загрузки кучи и определить поколение для очистки
        free(gen)
        todo: проверить обьекты в куче после очистки на корректность

    4. gc.finallizeRef (heapAddr)
        найти ноду с таким адресом
        очистить и удалить ноду из дерева (и удалить потомков)? это может быть не root обьект

*/


/*________ SortedList__uintpt */

typedef struct  {
    uintptr_t array[LIST_DEFAULT_LEN];
    int index;
    int last_del_index;
    int count;
} SortedList__uintpt;

int SortedList__uintpt_compare(const void* a, const void* b) {
    uintptr_t ca = * ((uintptr_t*)a);
    uintptr_t cb = * ((uintptr_t*)b);

    if (ca == cb) return 0;
    else if (cb > ca) return -1;
    else return 1; 
}

SortedList__uintpt* SortedList__uintpt_init() {
    SortedList__uintpt* list = (SortedList__uintpt*)malloc(sizeof(SortedList__uintpt));
    list->index = 0;
    list->last_del_index = -1;
    list->count = 0;
}

void SortedList__uintpt_add(SortedList__uintpt* ls, uintptr_t elem) {
    
    if (ls->last_del_index != -1) {
        ls->array[ls->last_del_index] = elem;
        ls->last_del_index = -1;
    }
    else {
        ls->array[ls->index++] = elem;
    }
    
    ls->count++;
    qsort(&ls->array[0], ls->index, sizeof(uintptr_t), SortedList__uintpt_compare);
}

void SortedList__uintpt_dump(SortedList__uintpt* ls) {
    printf("dump list [");
    for(int i =0; i < ls->index; i++) {
        printf(" %i,", ls->array[i]);
    }
    printf("]\n");
}

void SortedList__uintpt_removeAt(SortedList__uintpt* ls, int index) {
    
    if (ls->count == 0) {
        return;
    }
    
    ls->array[index] = LIST_EMPTY_VAL;
    ls->last_del_index = index;
    ls->count--;

    qsort(&ls->array[0], ls->index, sizeof(uintptr_t), SortedList__uintpt_compare);
    ls->index--;
}

/*________ gcRefTreeNode */

typedef struct  {
    void* childrens; // todo: typeof? SortedList__treeNode
    uintptr_t* start;
    int size;
    bool meta_isAvailable;
} gcRefTreeNode;

const gcRefTreeNode _empty_refNode = {.size = 9999999, .start = (uintptr_t*)NULL, .childrens = NULL};

/*________ SortedList__treeNode */

typedef struct  {
    gcRefTreeNode array[LIST_DEFAULT_LEN];
    int index;
    int last_del_index;
    int count;
} SortedList__treeNode;

int SortedList__treeNode_compare(const void* a, const void* b) {
    gcRefTreeNode ca = * ((gcRefTreeNode*)a);
    gcRefTreeNode cb = * ((gcRefTreeNode*)b);

    if (ca.start == _empty_refNode.start) return -1;
    else if (cb.start == _empty_refNode.start) return 1;

    if (ca.start == cb.start) return 0;
    else if (cb.start > ca.start) return -1;
    else return 1; 
}

SortedList__treeNode* SortedList__treeNode_init() {
    // todo: switch to self malloc
    SortedList__treeNode* list = (SortedList__treeNode*)malloc(sizeof(SortedList__treeNode));
    list->index = 0;
    list->last_del_index = -1;
    list->count = 0;
}

void SortedList__treeNode_add(SortedList__treeNode* ls, gcRefTreeNode elem) {
    
    if (ls->last_del_index != -1) {
        ls->array[ls->last_del_index] = elem;
        ls->last_del_index = -1;
    }
    else {
        ls->array[ls->index++] = elem;
    }
    
    ls->count++;
    qsort(&ls->array[0], ls->index, sizeof(gcRefTreeNode), SortedList__treeNode_compare);
}

void SortedList__treeNode_dump(SortedList__treeNode* ls) {
    printf("dump list (%i) [", ls->index);
    for(int i =0; i < ls->index; i++) {
        printf(" %i(%i),", ls->array[i].size, ls->array[i].meta_isAvailable);
    }
    printf("]\n");
}

void SortedList__treeNode_removeAt(SortedList__treeNode* ls, int index) {
    
    if (ls->count == 0) {
        ls->index = 0;
        return;
    }


    ls->array[index] = _empty_refNode;
    ls->last_del_index = index;
    ls->count--;

    qsort(&ls->array[0], ls->index, sizeof(gcRefTreeNode), SortedList__treeNode_compare);
    ls->index--;
}

void SortedList__treeNode_clean(SortedList__treeNode* list) {
    
    for(int i = list->index-1; i > 0; i--) {
        SortedList__treeNode_removeAt(list, i);
    }

    list->count = 0;
    list->index = 0;
    list->last_del_index = -1;
}


/*________ gcHeapRefTree */

typedef struct {
    SortedList__treeNode* rootList;
} gcHeapRefTree;

gcHeapRefTree* gcHeapRefTree_init() {
    gcHeapRefTree* tree = (gcHeapRefTree*)malloc(sizeof(gcHeapRefTree));
    tree->rootList = SortedList__treeNode_init(); 

    //printf("gcHeapRefTree_init\n");

    return tree;
}

enum GC_STATUS {
    RUNNING = 0,
    INITIALIZED = 1,
    STOPPED = -1,
};

typedef struct {
    gcHeapRefTree* gen0_tree;
    gcHeapRefTree* gen1_tree;
    gcHeapRefTree* gen2_tree;

    gcHeapRefTree* heapTree;
    pthread_mutex_t _func_frame_mutex;

    pthread_t* backgroud_thread;

    enum GC_STATUS status;

} GC;


gcRefTreeNode* gcRefTreeNode_init(uintptr_t* start, int size) {
    gcRefTreeNode* node = (gcRefTreeNode*)malloc(sizeof(gcRefTreeNode));
    node->childrens = (void*) SortedList__treeNode_init();
    node->size = size;
    node->start = start;
    node->meta_isAvailable = false;
}

int gcRefTreeNode_addChild(gcRefTreeNode* root, uintptr_t* child_start, int child_size) {
    SortedList__treeNode* list = (SortedList__treeNode*)(root->childrens);
    
    gcRefTreeNode node = {
        .childrens = (void*)SortedList__treeNode_init(),
        .size = child_size,
        .start = child_start,
        .meta_isAvailable = false
    };

    SortedList__treeNode_add(list, node);

    return list->index - 1;
}

gcRefTreeNode* gcRefTreeNode_getChild(gcRefTreeNode* root, int index) {
    SortedList__treeNode* list = (SortedList__treeNode*)(root->childrens);
    
    if (list->index <= index) {
        return NULL;
    }

    return &(list->array[index]);
}

int treeNode_minIndex(SortedList__treeNode* list, uintptr_t* target) {
    
    if (list->index == 0) {
        return -1;
    }
    if (list->index == 1) {
        return 0;
    }

    int start = 0;
    int end = list->index;
    int prev = -1;
    int mid = (start + end) / 2;

    // чекнуть входил ли элемент в выделенное пространство

    while(start < end) {

        mid = (start + end)/2;

        if (list->array[mid].start == target) {
            //printf("equal=%i\n", mid);
            return mid;
        }
        else if (list->array[mid].start < target) {
            start = mid;
        }
        else if (list->array[mid].start > target) {
            end = mid;
        }

        if (prev == mid) {
            
            // check start to mid
            if (start > 0) {
                start--;
            }
            
            if (list->array[mid + 1].start == target) {
                return mid +1;
            }
            else return 0;
        }
        else {
            prev = mid;
        }
    }
}

gcRefTreeNode* gcRefTreeNode_findNode(gcRefTreeNode* root, uintptr_t* target) {
    // todo: maybe return heapChunk

    if (root == NULL || root->childrens == NULL) return NULL;
    SortedList__treeNode* list = (SortedList__treeNode*)root->childrens;

    //printf("ref=%p index=%i\n", root, list->index);
    //SortedList__treeNode_dump(list);

    if (list->index == 0) return NULL;

    int bestIndex = treeNode_minIndex(list, target);

    if (list->array[bestIndex].start == target) {
        return &(list->array[bestIndex]);
    }
    else {
        while (list->index > bestIndex && list->array[bestIndex].start < target) {
            //printf("ref2=%p\n", root);
            gcRefTreeNode* deep = gcRefTreeNode_findNode(&(list->array[bestIndex]), target);
            
            if (deep != NULL) {
                return deep;
            }
            
            bestIndex++;
        }

        return NULL;
    }
}

gcRefTreeNode* gcRefTreeNode_findNodeFromTree(gcHeapRefTree* tree, uintptr_t* target) {
    // todo: maybe return heapChunk

    if (tree->rootList->index == 0) return NULL;

    SortedList__treeNode* list = tree->rootList;

    int bestIndex = treeNode_minIndex(list, target);

    if (bestIndex == -1) return NULL;

    if (list->array[bestIndex].start == target) {
        
        return &(list->array[bestIndex]);
    }
    else {
        //printf("META ELSE=%i \n", bestIndex);
        //SortedList__treeNode_dump(list);
        while (list->index > bestIndex && list->array[bestIndex].start < target) {
            
            gcRefTreeNode* deep = gcRefTreeNode_findNode(&(list->array[bestIndex]), target);
            
            if (deep != NULL) {
                return deep;
            }
            
            bestIndex++;
        }

        return NULL;
    }
}
 
void gcRefTreeNode_makeChildrens( gcRefTreeNode* node, SortedList__treeNode* child_obj_list) {
    SortedList__treeNode* list = (SortedList__treeNode*)node->childrens;
    SortedList__treeNode_clean(list);

    for(int i = 0; i < child_obj_list->index; i++) {
        gcRefTreeNode_addChild(
            node,
            child_obj_list->array[i].start,
            child_obj_list->array[i].size);
    }
}

void SortedList__treeNode_deepClean(SortedList__treeNode* list) {
    
    for(int i = 0; i < list->index; i++) {
        SortedList__treeNode* ls = (SortedList__treeNode*)list->array[i].childrens;
        SortedList__treeNode_deepClean(ls);
    }

    SortedList__treeNode_clean(list);
}

void SortedList__treeNode_removeNodeAt(SortedList__treeNode* list, int index) {
    if (index >= 0 && index < list->index) {
        SortedList__treeNode_deepClean( (SortedList__treeNode*)(list->array) );
        SortedList__treeNode_removeAt(list, index);
    }
}

GC main_gc;
extern Mheap heap;

void* _rsp;
void* _ebp;


/*
    TODO_MAIN:   
    - обновить поколения ПОСЛЕ обработки gc ЛИБО ПЕРЕСТАВИТЬ МЕСТАМИ gen2->gen1->gen0
    - добавить _free();


    - gc.nextGen() +
    - gc.freeNode()
    - gc.getHeapAllocStat()
    - gc.finallize
    - обновление фрейма
    - обновить зависимости и ссылки
    - если кто-то ссылается на поле другой ноды
    - убрать приписку gc
    - обработка NULL
    ---------------------
    - gc. вынести в фон
    - проверить на тестах
    - добавить мутекс?
    - рефакторить
    --------------------
    - почистить код
    - замерить перфоманс
    -------------------
    - перенести обьекты в след поколение
    - когда строить кучу
    - перенести построение кучи в фон
    - где вызывать методы кучи
    - clean comments/prints
*/

void gc_toNextGen(gcRefTreeNode* root) {
    
    /*
        Отправляет ноду в след поколение
    */

    printf("__gc_toNextGen: node=%p\n", root->start);
    int bestMin = treeNode_minIndex(main_gc.gen1_tree->rootList, root->start);

    // already in gen1
    if (main_gc.gen1_tree->rootList->array[bestMin].size == root->size &&
        main_gc.gen1_tree->rootList->array[bestMin].start == root->start) {

        int bestMin2 = treeNode_minIndex(main_gc.gen2_tree->rootList, root->start);

        if (main_gc.gen2_tree->rootList->array[bestMin].size != root->size ||
            main_gc.gen2_tree->rootList->array[bestMin].start != root->start) {

            SortedList__treeNode_add(main_gc.gen2_tree->rootList, *root);
        }
    } 
    else  SortedList__treeNode_add(main_gc.gen1_tree->rootList, *root);
}



int heapTree_pullChildrens(Mheap* heap, gcRefTreeNode* node) {
    
    SortedList__treeNode* ls = (SortedList__treeNode*)node->childrens;
    SortedList__treeNode_clean(ls); 

    int cursor = node->size;

    //uintptr_t* end = (uintptr_t*)((*(node->start)) + node->size);
    //printf("----END=%p END+1=%p", end, (end+1));

    if (node->size < 8) return;

    void* ptr = (void*)(node->start);

    while(cursor % 8 == 0 && cursor > 0) {
        
        int findIndex = _heap_indexOfChunk(*(uintptr_t*)ptr, 0, heap->alloced_index);

        if (findIndex != -1) {

            //printf("___ref: %p index=%i ref=%p\n", ptr, findIndex, heap->alloced_chunks[findIndex].start_at);

            // FIX: to refs
            gcRefTreeNode* crtNode = gcRefTreeNode_init(
                heap->alloced_chunks[findIndex].start_at,
                heap->alloced_chunks[findIndex].size);

            // FIX: to refs
            SortedList__treeNode_add(ls, *crtNode);
        }

        //printf("----CURSOR REF=%p  REF2=%p REF3=%p\n", ptr, *(uintptr_t*)ptr, &(*ptr));
        ptr += 8;
        cursor -= 8;
    }

    return ls->count;
}

void heapTree_makeNode(Mheap* heap, gcHeapRefTree* target_tree, size_t size, uintptr_t* start) {
    
    gcRefTreeNode* find = gcRefTreeNode_findNodeFromTree(target_tree, start);

    // add in roots
    if (find == NULL) {
        //printf("heapTree_makeNode: add root \n");
        // TODO: memory leek. сделать листы через ссылки
        gcRefTreeNode* node = gcRefTreeNode_init(start, size);
        //printf("ADD: %p\n",start);
        SortedList__treeNode_add(target_tree->rootList, *node);
        heapTree_pullChildrens(heap, node);
        SortedList__treeNode* nodels = (SortedList__treeNode*)node->childrens; 
        //printf("---------------------childrens=%p \n", nodels->array[0].start);
    }
    // add childrens
    else {
        //printf("heapTree_makeNode: add childrens \n");
        // pull childrens
        // FIX: DETERMINE MHeap CONTROL
        heapTree_pullChildrens(heap, find);
    }
}

void gcdump_tree(gcHeapRefTree* tree) {
    //ap_array();
    printf("gen_tree: [");
    for(int i = 0; i < tree->rootList->index; i++) {
        //SortedList__treeNode* ls = (SortedList__treeNode*)(tree->rootList->array[i].childrens);
        printf("%p(%i)[%i], ", tree->rootList->array[i].start, tree->rootList->array[i].size, 0);
    }
    printf("]\n");
}

void heapTree_build(Mheap* heap, gcHeapRefTree* heap_tree_gen) {
    /*
        пробегает по чанку, от начала до конца его выделенного адреса
        сканирует есть ли в выделенном пространстве адрес в кучу (ссылка на другой обьект в куче)
    */

    //TODO-CRIT: сделать так чтобы дерево дополнялось, а не очищалось

    printf("heapTree_build: \n");
    //SortedList__treeNode_dump(heap_tree_gen->rootList);

    for(int i = 0; i < heap->alloced_index; i++) {
        //printf("== %p \n", heap->alloced_chunks[i].start_at);
        
        heapTree_makeNode(
            heap,
            heap_tree_gen, 
            heap->alloced_chunks[i].size,
            heap->alloced_chunks[i].start_at);
    }
}


enum GC_NODE_STATUS {
    CAN_FREED = -1,
    PARSE_DEEP = 2,
    NOT_INITIALIZED = 0,
    REF_AVAILABLE = 1,
    PARSE_NEXT = 3
};

// TODO: убрать некоторые функи к gcnode

void gc_freeNode(gcRefTreeNode* root) {
    /*
        удалить ноду из дерева
        пройтись и зафришить 
    */

    SortedList__treeNode* list = (SortedList__treeNode*)root->childrens;
    //printf("____gc_freeNode: ptr=%p, count=%i \n", root->start, list->index);

    for(int i = 0; i < list->index; i++) {
        gc_freeNode(&(list->array[i]));
    }

    _free((void*)root->start);
}

enum GC_NODE_STATUS gcMarker_parseNode(gcRefTreeNode* root) {
    SortedList__treeNode* list = (SortedList__treeNode*)(root->childrens);
    //printf("gcMarker_parseNode: aval=%i  %p \n", root->meta_isAvailable, root->start);
    if (list->index == 0) return PARSE_NEXT;

    if (root->meta_isAvailable) return REF_AVAILABLE;

    bool any_available = false;
    for(int i = 0; i < list->index; i++) {
        if (list->array[i].meta_isAvailable == true) {
            // to next gen
            return REF_AVAILABLE;
        }
        else {

            enum GC_NODE_STATUS st = gcMarker_parseNode(&(list->array[i]));
            
            if (st == PARSE_NEXT) {
                i++;
            }
            else if (st == REF_AVAILABLE) {
                // to next gen
                return REF_AVAILABLE;
            }
        }
    }

    return PARSE_NEXT;
}

void gcTree_clean(gcHeapRefTree* tree, int end) {
    
    for(int i = 0; i < end; i++) {
        SortedList__treeNode_removeNodeAt(tree->rootList, i);
    }

    //printf("gcTree_clean: %i\n", tree->rootList->index);
    //SortedList__treeNode_dump(tree->rootList);
}

void gcMarker_parseTree(gcHeapRefTree* gen_tree) {
    /*
        бежит по маркнутому дереву и определяет обьекты
            либо в след поколения
            либо в очередь на очистку 
    */

    // todo: list<int>

    SortedList__treeNode* free_queue = SortedList__treeNode_init();

    int i = 0;
    for(; i < gen_tree->rootList->index; i++) {

        //printf("ROOT_NODE: %p  (%i) \n", gen_tree->rootList->array[i].start, gen_tree->rootList->array[i].meta_isAvailable);

        if (gen_tree->rootList->array[i].meta_isAvailable) {
            //printf("gcMarker_parseTree: next gen (ROOT) %p \n", gen_tree->rootList->array[i].start);
            gc_toNextGen(&(gen_tree->rootList->array[i]));
            //SortedList__treeNode_removeNodeAt(gen_tree->rootList, i);
            //SortedList__treeNode_add(free_queue, gen_tree->rootList->array[i]);
        }
        else {
            //printf("gcMarker_parseTree: (PARSE DEEP) %i \n", gen_tree->rootList->array[i].size);
            // прошлис по всей ноде, если stat == parseNext, можно фришить
            enum GC_NODE_STATUS stat = gcMarker_parseNode(&(gen_tree->rootList->array[i]));
            if (stat == REF_AVAILABLE) {
               
                gc_toNextGen(&(gen_tree->rootList->array[i]));
                //SortedList__treeNode_removeNodeAt(gen_tree->rootList, i);
                //SortedList__treeNode_add(free_queue, gen_tree->rootList->array[i]);
            }
            else if (stat == PARSE_NEXT) {
                // freed
                gc_freeNode(&(gen_tree->rootList->array[i]));
            }
        }
    }

    gcTree_clean(gen_tree, gen_tree->rootList->index);
}


void gcMarker_scanFrame(gcHeapRefTree* gen_tree, void* start, void* end) {

    /*
        определяет используемые (достижимые ) обьекты в данный момент на стеке
        остальыные обьекты остаются с меткойц как недостижимые (node.isAvailable)

        пробежать по стеку и определить достижимые указатели
    */

    while(start <= end) {

        if (((uintptr_t)start) % 8 == 0) {
            void* pt = (void*)start;
            //printf("___________________ref=%p\n", *(uintptr_t*)pt);
            //printf("CURSOR=%p REF=%p\n", start, *(uintptr_t*)pt);
            gcRefTreeNode* find = gcRefTreeNode_findNodeFromTree(gen_tree, *(uintptr_t*)pt);
            if (find != NULL) {
                //printf("FINDED=%p \n", find->start);
                find->meta_isAvailable = true;
            }
        }

        start+=1;
    }
}


enum GC_CLEANUP_STATUS {
    GENERATION_0 = 0,
    GENERATION_1 = 1,
    GENERATION_2 = 2
};

// 1024
// 50

void gcHeap_log(Mheap* heap ) {
    double percent = ((double)(heap->allocated_cap) / (double)(heap->max_cap)) * 100;
    printf("logs.gcHeap: total[%i] freed[%i] alloced_bytes=%i max_bytes=%i load%%=%f \n", heap->alloced_index, heap->freed_now, heap->allocated_cap, heap->max_cap, percent);
}

enum GC_CLEANUP_STATUS gcheap_GetCleanupStatus(Mheap* heap) {
    double percent = ((double)(heap->allocated_cap) / (double)(heap->max_cap)) * 100;

    return GENERATION_2;

    if (percent >= 90) {
        return GENERATION_2;
    }
    else if (percent >= 80) {
        return GENERATION_1;
    }
    else if (percent >= 75 ){
        return GENERATION_0;
    }
}


// finalize_gen
// findlize_ref
// finalize

void gc_finallizeTree(gcHeapRefTree* gen_tree) {

    //heapTree_build(&heap, gen_tree);

    gcMarker_scanFrame(gen_tree, _rsp, _ebp);

    gcMarker_parseTree(gen_tree);
}

void gc_finallize() {

    gcHeap_log(&heap);
    enum GC_CLEANUP_STATUS alloced_clean_status = gcheap_GetCleanupStatus(&heap);

    if (alloced_clean_status == GENERATION_2) {
        gc_finallizeTree(main_gc.gen2_tree);
        gc_finallizeTree(main_gc.gen1_tree);
        gc_finallizeTree(main_gc.gen0_tree);
    }
    else if (alloced_clean_status == GENERATION_1) {
        gc_finallizeTree(main_gc.gen1_tree);
        gc_finallizeTree(main_gc.gen0_tree);
    }
    else if (alloced_clean_status == GENERATION_0) {
        gc_finallizeTree(main_gc.gen0_tree);
    }

    gcHeap_log(&heap);
    //heapTree_build(&heap, main_gc.gen0_tree);
    //gcMarker_scanFrame(main_gc.gen0_tree, _rsp, _ebp);
    //gcMarker_parseTree(main_gc.gen0_tree);
    
   // join_thr?
}





void gc_init(GC* gc) {
    gc->gen0_tree = gcHeapRefTree_init();
    gc->gen1_tree = gcHeapRefTree_init();
    gc->gen2_tree = gcHeapRefTree_init();

    gc->heapTree = gcHeapRefTree_init();
    gc->_func_frame_mutex = PTHREAD_MUTEX_INITIALIZER;
    gc->backgroud_thread = (pthread_t*)malloc(sizeof(pthread_t));
    gc->status = INITIALIZED;
}


// TODO: mutex pthread_t* gc_thread
void* gc_scan() {

    // join_thr?

    int pivot = 0;

    void* prev_rsp = NULL;
    void* prev_ebp= NULL;
    while(true) {

        void* rsp = _rsp;
        void* ebp = _ebp;

        if (prev_ebp != ebp || prev_rsp != rsp) {

            if (ebp > rsp) {
                pivot++;
                printf("(frame) rsp: %p ebp: %p, diff %i \n", rsp, ebp, (void*)ebp - (void*)rsp);
                // TODO: периодически билдить и очищать
                heapTree_build(&heap, main_gc.gen0_tree);

                if (pivot == 5) {
                    gc_finallize();
                    pivot = 0;
                }
            }

            prev_ebp = ebp;
            prev_rsp = rsp;
        }
    }

    // join_thr?
}

void gc_start() {
    
    if (main_gc.status == INITIALIZED) {
        printf("GarbageCollector: starting in background..\n");
        pthread_create(main_gc.backgroud_thread, NULL, gc_scan, NULL); 
    }
}


typedef struct {
    int f;
    int* pt1;
    int* pt2;
} Foo;

typedef struct {
    int f;
    int* pt1;
    int* pt2;
    Foo* pt3;
} Bar;


Foo* bar() {
    
    Foo* ptr2 = (Foo*)_malloc(sizeof(Foo));
    ptr2->pt2 = _malloc(4);
    ptr2->pt1 = _malloc(4);

    int* t1 = _malloc(4);

    Bar* br = (Bar*)_malloc(sizeof(Bar));
    br->pt1 = _malloc(sizeof(int));
    br->pt2 = _malloc(sizeof(int));

    return ptr2;
}

void func() {
    Foo* ptr2 = (Foo*)_malloc(sizeof(Foo));
    ptr2->pt2 = _malloc(4);
    ptr2->pt1 = _malloc(4);

    int* t1 = _malloc(4);

    Bar* br = (Bar*)_malloc(sizeof(Bar));
    br->pt1 = _malloc(sizeof(int));
    br->pt2 = _malloc(sizeof(int));

    GC_SET_FRAME;
}


int main() {

    GC_SET_FRAME;
    printf("STACK FRAME: %p ___ %p DIFF=%i \n", _rsp, _ebp, (int )(_ebp-_rsp));

    gc_init(&main_gc);

    gc_start();

    double* ptr = _malloc(sizeof(double));

    Foo * ptr3 = bar();
    bar();
    func();
    bar();
    func();
    bar();
    func();
    bar();

    //GC_SET_FRAME;

    sleep(5);

    return 0;

}