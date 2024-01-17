#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

// todo: reserve some heap to gc

#define LIST_DEFAULT_LEN 200

#define LIST_EMPTY_TREENODE 
#define LIST_EMPTY_VAL 9000000000000000

// todo: flexible capacity
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

void SortedList__uintpt_init(SortedList__uintpt* list) {
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

//SortedList__uintpt ls;

// add sortedList<gc_TreeNode>
// add gc.reserveRef(ref)
// add gc main class

//------------------------------------

typedef struct  {
    void* childrens;
    void* start;
    int size;
} gc_RefTreeNode;


typedef struct  {
    gc_RefTreeNode array[LIST_DEFAULT_LEN];
    int index;
    int last_del_index;
    int count;
} SortedList__treeNode;



SortedList__treeNode temp_list;

int SortedList__treeNode_compare(const void* a, const void* b) {
    gc_RefTreeNode ca = * ((gc_RefTreeNode*)a);
    gc_RefTreeNode cb = * ((gc_RefTreeNode*)b);

    if (ca.size == -1) return -1;
    else if (cb.size == -1) return 1;

    if (ca.size == cb.size) return 0;
    else if (cb.size > ca.size) return -1;
    else return 1; 
}

void SortedList__treeNode_init(SortedList__treeNode* list) {
    list->index = 0;
    list->last_del_index = -1;
    list->count = 0;
}

void SortedList__treeNode_add(SortedList__treeNode* ls, gc_RefTreeNode elem) {
    
    if (ls->last_del_index != -1) {
        ls->array[ls->last_del_index] = elem;
        ls->last_del_index = -1;
    }
    else {
        ls->array[ls->index++] = elem;
    }
    
    ls->count++;
    qsort(&ls->array[0], ls->index, sizeof(gc_RefTreeNode), SortedList__treeNode_compare);
}

void SortedList__treeNode_dump(SortedList__treeNode* ls) {
    printf("dump list [");
    for(int i =0; i < ls->index; i++) {
        printf(" %i,", ls->array[i].size);
    }
    printf("]\n");
}

// todo: sortedList.array сделать поинтеры вместо значений 

gc_RefTreeNode default_node = {.size = 999, .start = NULL};

void SortedList__treeNode_removeAt(SortedList__treeNode* ls, int index) {
    
    if (ls->count == 0) {
        return;
    }
    
    ls->array[index] = default_node;
    ls->last_del_index = index;
    ls->count--;

    SortedList__treeNode_dump(&temp_list);


    qsort(&ls->array[0], ls->index, sizeof(uintptr_t), SortedList__treeNode_compare);
    
    SortedList__treeNode_dump(&temp_list);
    ls->index--;
}





gc_RefTreeNode tr;

void init(gc_RefTreeNode* tree) {
    tree->childrens = &temp_list;
}

int main() {

    SortedList__treeNode_init(&temp_list);

    init(&tr);

    gc_RefTreeNode s1 = {.size = 1, .start = NULL};
    gc_RefTreeNode s2 = {.size = 2, .start = NULL};
    gc_RefTreeNode s3  = {.size = 3, .start = NULL};

    SortedList__treeNode* list = (SortedList__treeNode*)tr.childrens;
    printf("%i \n", list->array[0]);

    SortedList__treeNode_add(&temp_list, s1);
    SortedList__treeNode_add(&temp_list, s2);
    SortedList__treeNode_add(&temp_list, s3);

    SortedList__treeNode_removeAt(&temp_list, 1);

    SortedList__treeNode_dump(&temp_list);

    printf("%i \n", list->count);


    return 0;

}