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
/*
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
} gcRefTreeNode;

const gcRefTreeNode _empty_refNode = {.size = 9999999, .start = (uintptr_t*)NULL};

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


/*________ gcHeapRefTree */

typedef struct {
    SortedList__treeNode* rootList;
} gcHeapRefTree;

gcHeapRefTree* gcHeapRefTree_init() {
    gcHeapRefTree* tree = (gcHeapRefTree*)malloc(sizeof(gcHeapRefTree));
    tree->rootList = SortedList__treeNode_init(); 

    printf("gcHeapRefTree_init\n");

    return tree;
}

typedef struct {
    gcHeapRefTree* gen0_tree;
    gcHeapRefTree* gen1_tree;
    gcHeapRefTree* gen2_tree;

    gcHeapRefTree* heapTree;
    pthread_mutex_t _func_frame_mutex;
} GC;


gcRefTreeNode* gcRefTreeNode_init(uintptr_t* start, int size) {
    gcRefTreeNode* node = (gcRefTreeNode*)malloc(sizeof(gcRefTreeNode));
    node->childrens = (void*) SortedList__treeNode_init();
    node->size = size;
    node->start = start;
}

int gcRefTreeNode_addChild(gcRefTreeNode* root, uintptr_t* child_start, int child_size) {
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

int treeNode_minIndex(SortedList__treeNode* list, uintptr_t* target) {
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

            void* scalp_tg = (void*)target;

            /*
            // ПРОБЕГАЕМ ПО ДИАПАЗОНУ АДРЕСОВ
            // TODO: chunk metafield {last_addr} чтобы определять диапазон адресов
            while(start <= mid) {
                

                while()

                void* st = list->array[start].start;
                void* en = st + 8;

                printf("____________[window] %p --- %p . tar=%p \n", st, en, scalp_tg);
                if (st > scalp_tg && scalp_tg < en) {
                    return start;
                }

                start++;
            }
            */
            
            if (list->array[mid + 1].start == target) {
                return mid +1;
            }
            else return 0;
        }
        else {
            prev = mid;
        }

        //printf("mid=%i\n", mid);
    }
}

gcRefTreeNode* gcRefTreeNode_findNode(gcRefTreeNode* root, uintptr_t* target) {
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

gcRefTreeNode* gcRefTreeNode_findNodeFromTree(gcHeapRefTree* tree, uintptr_t* target) {
    // todo: maybe return heapChunk
    SortedList__treeNode* list = (SortedList__treeNode*)tree->rootList;

    //printf("___gcRefTreeNode_findNodeFromTree IN\n");

    int bestIndex = treeNode_minIndex(list, target);

    printf("___gcRefTreeNode_findNodeFromTree bestMin=%i target=%p \n", bestIndex, target);

    //dump_chunks();

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



GC main_gc;
extern Mheap heap;

void* _rsp;
void* _ebp;


/*
    TODO_MAIN:   
    - маркнуть активные обьекты
    - определить обьекты для освобождения 
    - перенести обьекты в след поколение
    - когда строить кучу
    - перенести построение кучи в фон
    - где вызывать методы кучи
*/

void gc_mark() {
    // mark current rootList
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
    
    if (find != NULL) {
        printf("heapTree_makeNode: start=%p find=%p \n", start, find->start);
    }
    else {
        printf("heapTree_makeNode: start=%p find=NULL \n", start);
    }

    //gcdump_tree();

    // add in roots
    if (find == NULL) {
        //printf("heapTree_makeNode: add root \n");
        // TODO: memory leek. сделать листы через ссылки
        gcRefTreeNode* node = gcRefTreeNode_init(start, size);
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

void gcdump_tree() {
    //ap_array();
    printf("gen0_tree: [");
    for(int i = 0; i < main_gc.gen0_tree->rootList->index; i++) {
        SortedList__treeNode* ls = (SortedList__treeNode*)(main_gc.gen0_tree->rootList->array[i].childrens);
        printf("%p(%i)[%i], ", main_gc.gen0_tree->rootList->array[i].start, main_gc.gen0_tree->rootList->array[i].size, ls->count);
    }
    printf("]\n");
}

void heapTree_build(Mheap* heap, gcHeapRefTree* heap_tree_gen) {
    /*
        пробегает по чанку, от начала до конца его выделенного адреса
        сканирует есть ли в выделенном пространстве адрес в кучу (ссылка на другой обьект в куче)
    */

    printf("___heapTree_build index=%i\n", heap->alloced_index);
    //dump_heap_array();

    for(int i = 0; i < heap->alloced_index; i++) {
        //printf("== %p \n", heap->alloced_chunks[i].start_at);
        
        heapTree_makeNode(
            heap,
            heap_tree_gen, 
            heap->alloced_chunks[i].size,
            heap->alloced_chunks[i].start_at);
    }
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




void gc_finallize() {
    
    //
    
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





void gc_init(GC* gc) {
    gc->gen0_tree = gcHeapRefTree_init();
    gc->gen1_tree = gcHeapRefTree_init();
    gc->gen2_tree = gcHeapRefTree_init();

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

            //printf("(frame) rsp: %p ebp: %p, diff %i \n", rsp, ebp, (void*)ebp - (void*)rsp);

            if (ebp > rsp) {
                printf("(frame) rsp: %p ebp: %p, diff %i \n", rsp, ebp, (void*)ebp - (void*)rsp);
                // TODO: периодически вызывать build_tree
                //heapTree_build(&heap, main_gc.gen0_tree);
            }

            prev_ebp = ebp;
            prev_rsp = rsp;
        }
    }

    // join_thr?
}

void gc_start(pthread_t* gc_thread) {
    // todo: field status enum
    pthread_create(gc_thread, NULL, gc_scan, NULL);
}

void gc_updateFuncFrame() {
    pthread_mutex_lock((&main_gc._func_frame_mutex));
    register void* rsp __asm__("rsp");
    register void* ebp __asm__("ebp");
    _rsp = rsp;
    _ebp = ebp;
    pthread_mutex_unlock((&main_gc._func_frame_mutex));
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
    int* pt3;
} Bar;


int bar() {
    void* f1;
    void* f2;
    void* f3;
    void* f4;
    void* f5;
    //double* ptr = (double*)_malloc(sizeof(double));
    Foo* ptr2 = (Foo*)_malloc(sizeof(Foo));
    ptr2->pt2 = _malloc(4);

    //Bar* ptr3 = (Bar*)_malloc(sizeof(Bar));


    double* t1 = _malloc(8);

    ptr2->pt1 = _malloc(4);

    Bar* bar = _malloc(sizeof(Bar));
    bar->pt1 = _malloc(4);
    bar->pt3 = _malloc(4);


    GC_SET_FRAME;
    return 5;
}

int main3() { 

    int* p2 = (int*)_malloc(sizeof(int));
    Foo* p4 = (Foo*)_malloc(sizeof(Foo));
    p4->pt1 = _malloc(sizeof(int));
    printf("void*=%p, val=%p \n", p4, *(uintptr_t*)(p4));
    p4+=8;
    printf("+8 void*=%p, val=%p \n", p4, *(uintptr_t*)(p4));
    p4+=8;
    printf("+8 void*=%p, val=%p \n", p4, *(uintptr_t*)(p4));
    p4+=16;
    printf("+16 void*=%p, val=%p \n", p4, *(uintptr_t*)(p4));
    int* p5 = (int*)_malloc(sizeof(int));

    //Foo* p1 = (Foo*)_malloc(sizeof(Foo*));
    //p1->pt1 = _malloc(4);

    //int* p2 = (int*)_malloc(sizeof(int));

    dump_chunks();


    //printf("void*=%p, val=%p \n", p1, *(uintptr_t*)(p1));
    //printf("void*=%p, val=%p \n", p1->pt1, *(uintptr_t*)(p1->pt1));

    //heap_shift_left(1, 1);

    // 1 2 1 2(0)

    //printf("res=%i \n", _heap_indexOfChunk((void*)p1, 0, heap.alloced_index));

    return 0;
}

int main() {



    gc_init(&main_gc);

    long* ptr = _malloc(sizeof(long));

    GC_SET_FRAME;

    dump_heap_array();

    pthread_t secId;
    gc_start(&secId);

    sleep(1);

    bar();

    heapTree_build(&heap, main_gc.gen0_tree);

    gcdump_tree();
    dump_chunks();

    sleep(10);


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