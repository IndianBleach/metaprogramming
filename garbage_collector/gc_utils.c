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
    SortedList__treeNode* list = (SortedList__treeNode*)root->childrens;

    int bestIndex = treeNode_minIndex(list, target);

    if (list->array[bestIndex].start == target) {
        return &(list->array[bestIndex]);
    }
    else {
        while (list->array[bestIndex].start < target) {
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

    //printf("___heapTree_build index=%i\n", heap->alloced_index);
    //dump_heap_array();

    printf("heapTree_build: \n");
    SortedList__treeNode_dump(heap_tree_gen->rootList);

    for(int i = 0; i < heap->alloced_index; i++) {
        //printf("== %p \n", heap->alloced_chunks[i].start_at);
        
        heapTree_makeNode(
            heap,
            heap_tree_gen, 
            heap->alloced_chunks[i].size,
            heap->alloced_chunks[i].start_at);
    }
}


/*
        TODO:
        периодически собирать дерево? где лучший момент
        добавить в чанк meta.gc_status flag?

        СОБИРАТЬ ДЕРЕВО?
            - в фоне
            - конечный адрес в хипе для сборки (если на стеке есть ссылка то безсполезно) как не собрать лишнего?
            - chunk.gc_status (parsed, noparsed)

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
    printf("____gc_freeNode: ptr=%p, count=%i \n", root->start, list->index);

    for(int i = 0; i < list->index; i++) {
        gc_freeNode(&(list->array[i]));
    }
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
            SortedList__treeNode_add(free_queue, gen_tree->rootList->array[i]);
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
                printf("FINDED=%p \n", find->start);
                find->meta_isAvailable = true;
            }
        }

        start+=1;
    }
}





void gc_finallize() {
    
    heapTree_build(&heap, main_gc.gen0_tree);
    gcMarker_scanFrame(main_gc.gen0_tree, _rsp, _ebp);
    gcMarker_parseTree(main_gc.gen0_tree);
    
    //printf("DUMP GEN TREE: \n");
    //SortedList__treeNode_dump(main_gc.gen1_tree->rootList);

    //heapTree_build(&heap, main_gc.gen0_tree);
    //gcMarker_scanFrame(main_gc.gen1_tree, _rsp, _ebp);
    //gcMarker_parseTree(main_gc.gen1_tree);
    // join_thr?

    // cleanGen = heapAllocdProc()
    // gen0 65%
    // gen1 80%
    // gen2 90%

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

    printf("STACK FRAME: %p ___ %p DIFF=%i \n", _rsp, _ebp, (int )(_ebp-_rsp));
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


int main() {

    register void* rsp __asm__("rsp");
    register void* ebp __asm__("ebp");
    _rsp = rsp;
    _ebp = ebp;
    printf("STACK FRAME: %p ___ %p DIFF=%i \n", _rsp, _ebp, (int )(_ebp-_rsp));

    gc_init(&main_gc);

    double* ptr = _malloc(sizeof(double));
    //int* ptr2 = _malloc(sizeof(int));

    //GC_SET_FRAME;

    Foo * ptr3 = bar();


    //GC_SET_FRAME;

    gc_finallize();
    //SortedList__treeNode_dump(main_gc.gen0_tree->rootList);
    //SortedList__treeNode_dump(main_gc.gen1_tree->rootList);
    //SortedList__treeNode_dump(main_gc.gen0_tree->rootList);
    //SortedList__treeNode_dump(main_gc.gen2_tree->rootList);
    printf("------------------\n");
    //pthread_t secId;
    //gc_start(&secId);

    sleep(1);

    //bar();
    SortedList__treeNode_dump(main_gc.gen0_tree->rootList);
    SortedList__treeNode_dump(main_gc.gen1_tree->rootList);
    SortedList__treeNode_dump(main_gc.gen2_tree->rootList);
    printf("___________FINALLIIZE\n");
    gc_finallize();
    SortedList__treeNode_dump(main_gc.gen0_tree->rootList);
    SortedList__treeNode_dump(main_gc.gen1_tree->rootList);
    SortedList__treeNode_dump(main_gc.gen2_tree->rootList);

    //Bar* ptbar = bar();

    //gcdump_tree();

    sleep(10);

    return 0;

}