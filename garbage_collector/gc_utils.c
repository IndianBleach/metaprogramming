#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

#define LIST_DEFAULT_LEN 200

#define LIST_EMPTY_TREENODE 
#define LIST_EMPTY_VAL 9000000000000000

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
            printf("equal=%i\n", mid);
            return mid;
        }
        else if (list->array[mid].start < target) {
            start = mid;
        }
        else if (list->array[mid].start > target) {
            end = mid;
        }

        if (prev == mid) {
            printf("same=%i\n", mid);
            return mid;
        }
        else {
            prev = mid;
        }

        printf("mid=%i\n", mid);
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
            printf("index=%i\n", bestIndex);
            gcRefTreeNode* deep = gcRefTreeNode_findNode(&(list->array[bestIndex]), target);
            
            if (deep != NULL) {
                return deep;
            }
            
            bestIndex++;
        }

        return NULL;
    }
}

/*________ gcHeapRefTree */

typedef struct {
    SortedList__treeNode* rootList;
} gcHeapRefTree;

gcHeapRefTree* gcHeapRefTree_init() {
    gcHeapRefTree* tree = (gcHeapRefTree*)malloc(sizeof(gcHeapRefTree));
    tree->rootList = (void*) SortedList__treeNode_init(); 
}


int main() {


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

    return 0;

}