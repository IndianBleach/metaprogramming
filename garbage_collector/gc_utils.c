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


#define GC_SET_FRAME gc_updateFuncFrame();

const int STACK_FRAME_PTRDIFF = sizeof(uintptr_t);

extern int _heap_indexOfChunk(void* target, int startIndex, int endIndex);

// todo: sortedList flexible capacity
// add gc.reserveRef(ref)
// add gc main class
// add malloc realization with heap
// add gcHeapRefTree.addRoot?


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
    void* start;
    int size;
} gcRefTreeNode;

const gcRefTreeNode _empty_refNode = {.size = 9999999, .start = NULL};

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

    if (ca.start == -1) return -1;
    else if (cb.start == -1) return 1;

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
    printf("dump list [");
    for(int i =0; i < ls->index; i++) {
        printf(" %i,", ls->array[i].size);
    }
    printf("]\n");
}

void SortedList__treeNode_removeAt(SortedList__treeNode* ls, int index) {
    
    if (ls->count == 0) {
        return;
    }
    
    ls->array[index] = _empty_refNode;
    ls->last_del_index = index;
    ls->count--;

    qsort(&ls->array[0], ls->index, sizeof(gcRefTreeNode), SortedList__treeNode_compare);
    ls->index--;
}

void SortedList__treeNode_clean(SortedList__treeNode* list) {
    list->count = 0;
    list->index = 0;
    list->last_del_index = -1;
}




gcRefTreeNode* gcRefTreeNode_init(void* start, int size) {
    gcRefTreeNode* node = (gcRefTreeNode*)malloc(sizeof(gcRefTreeNode));
    node->childrens = (void*) SortedList__treeNode_init();
    node->size = size;
    node->start = start;
}

int gcRefTreeNode_addChild(gcRefTreeNode* root, void* child_start, int child_size) {
    SortedList__treeNode* list = (SortedList__treeNode*)(root->childrens);
    
    gcRefTreeNode node = {
        .childrens = (void*)SortedList__treeNode_init(),
        .size = child_size,
        .start = child_start
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

int treeNode_minIndex(SortedList__treeNode* list, void* target) {
    int start = 0;
    int end = list->index;
    int prev = -1;
    int mid = (start + end) / 2;
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
            //printf("same=%i\n", mid);
            return mid;
        }
        else {
            prev = mid;
        }

        //printf("mid=%i\n", mid);
    }
}

gcRefTreeNode* gcRefTreeNode_findNode(gcRefTreeNode* root, void* target) {
    // todo: maybe return heapChunk
    SortedList__treeNode* list = (SortedList__treeNode*)root->childrens;

    int bestIndex = treeNode_minIndex(list, target);

    if (list->array[bestIndex].start == target) {
        return &(list->array[bestIndex]);
    }
    else {
        while (list->array[bestIndex].start < target) {
            //printf("index=%i\n", bestIndex);
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



bool stack_anyRef(void* heapAddr) {
    // sliding window from rsp to ebp
    // check (*[window]) == heapAddr 
}


void gc_mark() {
    // mark current rootList
}

void gc_toNextGen(gcRefTreeNode* root) {
    
    int bestMin = treeNode_minIndex(main_gc.gen1, root->start);
    // todo: size&ptr good node identifier?
    // already exists in gen1
    // todo: add gen0
    // todo: remove from prev gen
    if (main_gc.gen1->array[bestMin].size == root->size &&
        main_gc.gen1->array[bestMin].start == root->start) {

            int bestMin2 = treeNode_minIndex(main_gc.gen2, root->start);
            if (main_gc.gen2->array[bestMin].size != root->size ||
                main_gc.gen2->array[bestMin].start != root->start) {
                SortedList__treeNode_add(main_gc.gen2, *root);
            }
    } 
    else {
        SortedList__treeNode_add(main_gc.gen1, *root);
    }
}




void heapTree_build(Mheap* heap) {
    /*
        пробегает по чанку, от начала до конца его выделенного адреса
        сканирует есть ли в выделенном пространстве адрес в кучу (ссылка на другой обьект в куче)
        
        makeNode()

        for chunk in heap
            
            pullChildrens( heap, node )
            makeNode( tree, node )

    */

    for(int i = 0; i < heap->alloced_index; i++) {
        // узнать есть ли уже в дереве
        // 
    }

}

void heapTree_makeNode(gcHeapRefTree* tree, size_t size, void* start) {
    // чекнуть есть ли уже среди корней
    // getChildrens
    // findNode
    // addRoot
    // 
}

// to makeNode
void heapTree_pullChildrens() {

}


void gc_scanFrame(void* start, void* end) {

    /*
        TODO:
        периодически собирать дерево? где лучший момент
        добавить в чанк meta.gc_status flag?

        СОБИРАТЬ ДЕРЕВО?
            - в фоне
            - конечный адрес в хипе для сборки (если на стеке есть ссылка то безсполезно) как не собрать лишнего?
            - chunk.gc_status (parsed, noparsed)

        gc.scanFrame()
            1. пройтись по списку адресов
            2. пометить эти ноды как .is_available

        gen0 tree (неотсканированное на основе кучи) построено в результате периодических вызовов
        gen1 tree (после 1 скана)
        gen2 tree (после 2 скана)

        gc.cleanTree()
            пробегает по дереву (по рутам), 
                очищает - если !root.is_available
                переносит в след поколение - если .is_available. gen1Tree.addRoot(root)
            gen0Tree.clean()

        gc.freeNode( root )
        gc.nextGen( node )

    */



    while(start <= end) {

        // get chunk
        int findIndex = _heap_indexOfChunk(start, 0, heap.alloced_index);

        // get depends refs
        if (findIndex != -1) {

        }

        start += STACK_FRAME_PTRDIFF;
    }

}


SortedList__uintpt* gcHeap_pullChildrens(Mheap* heap, int chunk_index) {
    SortedList__uintpt* ls = (SortedList__uintpt*)malloc(sizeof(SortedList__uintpt));
    HeapChunk chunk = heap->alloced_chunks[chunk_index];

    void* ptr = chunk.start_at;
    while(ptr <= (void*)(ptr+chunk.size)) {

        int findIndex = _heap_indexOfChunk(ptr, 0, heap->alloced_index);

        if (findIndex != -1) {
            //SortedList__uintpt_add(ls, )
        }

        ptr += 1;
    }
}



/*
    когда собирать дерево?
    как маркнуть обьекты в куче:
        предполагается что в дереве сейчас активные обьекты, остальные - нет
        -> начинаем скан в куче и сверяем с деревом

    ДЛЯ ОЧИСТКИ (С КУЧИ)
    1) начинаем бежать по куче (нужна метка на конец скана)
    2) у нас уже должны быть маркнуты root в дереве, которые могут быть очищены
    2) сверяем есть ли chunk.addr 
 
    FIX:
        - с кучи: заполнять деерво
        - с фрейма: маркать используемые обьекты в дереве

    node {
        start*
        end*
        is_available
        children_list 
    }


    1. checkFrame()
        _join_mainThread
        устанавливает взаимосвязь стека и ссылок в куче
        heapChunk = getFromHeap( *[stack_win] ) - получает чанк из кучи (bs)
        list<chunks> = getChildrenRefs( heapChunk ) - проходится по chunk.size и смотрит ссылки
        makeNode( heapChunk, list<chunks> childrens) - либо создаёт root, либо получает, либо прикрепляем ноду к дереву вместе с дочерними элементами
        markNode( node ) - помечает только текущий нод, без дочерних
        _join_mainThread
        ----------------
        построенное дерево с помеченными элементами, которые используется сейчас на стеке (помечены available)

    2. gc.mark()
        пробегается по текущему дереву и смотрит на метку node.available
        помечает листья и корни, если в результате скана корень не имеет available 
            -> добавить нод в очередь на очистку
            -> либо в след поколение
        --------------------
        фришные ноды будут удалены из дерева rootList
        нефришные ноды будут перемещены в gen0

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


void gc_finallize() {
    // mark 1gen
    
    // join_thr?

    // cleanGen = heapAllocdProc()
    // gen0 65%
    // gen1 80%
    // gen2 90%

    /*
        for root in rootList
            if (root.isAvailable) toNextGen(node)
            else freeNode(root)

    */

   // join_thr?
}



/*________ gcHeapRefTree */

typedef struct {
    SortedList__treeNode* rootList;
} gcHeapRefTree;

gcHeapRefTree* gcHeapRefTree_init() {
    gcHeapRefTree* tree = (gcHeapRefTree*)malloc(sizeof(gcHeapRefTree));
    tree->rootList = SortedList__treeNode_init(); 

    return tree;
}

typedef struct {
    SortedList__treeNode* gen0;
    SortedList__treeNode* gen1;
    SortedList__treeNode* gen2;

    gcHeapRefTree* heapTree;
    pthread_mutex_t _func_frame_mutex;
} GC;

GC main_gc;
extern Mheap heap;

void* _rsp;
void* _ebp;


void gc_init(GC* gc) {
    gc->gen0 = SortedList__treeNode_init();
    gc->gen1 = SortedList__treeNode_init();
    gc->gen2 = SortedList__treeNode_init();

    gc->heapTree = gcHeapRefTree_init();
    gc->_func_frame_mutex = PTHREAD_MUTEX_INITIALIZER;
}

/*
    todo:
        - gc.scanFrame(start, end)
        - gc.makeChildrens
        - gc.mark()

        - stack.anyRef( addr )
         
        - GC.NextGen( root )

        - GC.Finallize
            // for i in 1gen, ...next
            _mark()
            _free()

        - GC.FinallizeObj( ref )
*/
 

void* gc_scan(pthread_t* gc_thread) {

    // join_thr?

    void* prev_rsp = NULL;
    void* prev_ebp= NULL;
    while(true) {

        //printf("_(frame) rsp: %p ebp: %p \n", prev_rsp, prev_ebp);

        void* rsp = _rsp;
        void* ebp = _ebp;
        //register void* ebp __asm__("ebp");
        
       //printf("___________(frame) rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);

        if (prev_ebp != ebp || prev_rsp != rsp) {
            
            printf("(frame) rsp: %p ebp: %p, diff %i \n", rsp, ebp, (void*)ebp - (void*)rsp);
            prev_ebp = ebp;
            prev_rsp = rsp;

            // TODO: периодически вызывать build_tree

        }
    }

    // join_thr?
}

void gc_start(pthread_t* gc_thread) {
    // todo: field status enum
    pthread_create(gc_thread, NULL, gc_scan, NULL);
}




void gc_updateFuncFrame() {
    pthread_mutex_lock(&(main_gc._func_frame_mutex));
    register void* rsp __asm__("rsp");
    register void* ebp __asm__("ebp");
    _rsp = rsp;
    _ebp = ebp;
    pthread_mutex_unlock(&(main_gc._func_frame_mutex));
}




int bar() {
    GC_SET_FRAME;
    return 5;
}





int main() {

    gc_init(&main_gc);

    printf("%i\n", main_gc.heapTree->rootList->count);

    pthread_t secId;
    gc_start(&secId);

    pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&new_mutex);
    register void* rsp __asm__("rsp");
    register void* ebp __asm__("ebp");
    _rsp = rsp;
    _ebp = ebp;
    pthread_mutex_unlock(&new_mutex);
    
    sleep(1);

    bar();

    sleep(3);


    /*
        int* p1 = (int*)_malloc(sizeof(int));

    dump_heap_array();


    gcRefTreeNode* node = gcRefTreeNode_init(NULL, 1);

    // 1-100
    for(int i = 1; i < 100 ; i++) {
        //gcRefTreeNode nd = *(gcRefTreeNode_init((void*)(i), i));
        gcRefTreeNode_addChild(node, (void*)(i), i);
    }

    printf("13\n");

    // 10
    gcRefTreeNode* node2 = gcRefTreeNode_getChild(node, 98);

    // d2 100-126
    for(int i = 100; i < 126 ; i++) {
        gcRefTreeNode_addChild(node2, (void*)(i), i);
    }

    printf("2\n");

    // 10
    gcRefTreeNode* node3 = gcRefTreeNode_getChild(node2, 25);

    printf("list3=%i\n", node3->size);

    // d2 100-126
    for(int i = 126; i < 150 ; i++) {
        gcRefTreeNode_addChild(node3, (void*)(i), i);
    }


    void* tar = (void*)144;
    gcRefTreeNode* find = gcRefTreeNode_findNode(node, tar);

    printf("targ=%p finded=%p\n", tar, find->start);
   
    //printf("val=%p\n", (uintptr_t)(list->array[best].start));

    //SortedList__treeNode_dump(list);
    //SortedList__treeNode_init(&temp_list);

    */

    return 0;

}