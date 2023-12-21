#include <stdlib.h>
#include <stdio.h>





// List_int



struct LListNode_int
{
    int val;
    struct LListNode_int* next;   
};

struct LList_int 
{
    int len;
    
    struct LListNode_int* last;
    struct LListNode_int* first;

};

struct LList_int* LList_int_init() {
    struct LList_int* p = (struct LList_int*)malloc(sizeof(struct LList_int));
    p->len = 0;
    p->first = NULL;
    p->last = NULL;
}

struct LListNode_int* LListNode_int_init(int val, struct LListNode_int* next) {
    struct LListNode_int* p = (struct LListNode_int*)malloc(sizeof(struct LListNode_int));
    p->val = val;
    p->next = next;
}


// TODO: addRange(nodes...)

void LList_int_add(struct LList_int* list, int val) {
    if (list->first == NULL) {
        struct LListNode_int* p = LListNode_int_init(val, NULL);
        list->first = p;
        list->last = p;
    }
    else {
        list->last->next = LListNode_int_init(val, NULL);
        list->last = list->last->next;
    }

    list->len++;
}

int LList_int_removeAt(struct LList_int* list, int pos) {
    if (list->len < pos) {
        return -1;
    }

    // go to posElem
    // update next

    if (list->len == 1) {
        // clean
        return 0;
    }

    int cur_pos = 1;
    struct LListNode_int* cur = list->first;
    struct LListNode_int* tmp;

    while(cur->next != NULL) {
        cur_pos++;

        // exluce [cur->next]
        if (cur_pos == pos) {

            if (pos == list->len) {
                
                list->last = cur;
                list->last->next = NULL;

                free(cur->next);
                return 1;
            }
            else {
                tmp = cur->next;
                cur->next = cur->next->next;

                list->len--;

                free(tmp);
                return 1;
            }
        }
        else cur = cur->next; 
    }
}

void LList_int_printNodes(struct LList_int* ls) {
    struct LListNode_int* cur = ls->first;
    int pos = 1;
    while(cur != NULL) {
        printf("pos=%i val=%i\n", pos++, cur->val);
        cur = cur->next;
    }
}

void LList_int_clear(struct LList_int* ls) {
    
    struct LListNode_int* pts[ls->len];
    int tpos = 0;

    struct LListNode_int* cur = ls->first;
    while(cur != NULL) {
        pts[tpos++] = cur;
        cur = cur->next;
    }

    for(int i = 0; i < ls->len;i++) {
        free(pts[i]);
    }

    ls->first = NULL;
    ls->last = NULL;
    ls->len = 0;
}

// TODO: types to generic (preproc)

int main()
{
    struct LList_int* ls = LList_int_init();
    
    LList_int_add(ls, 1);
    LList_int_add(ls, 2);
    LList_int_add(ls, 3);
    LList_int_add(ls, 4);
    LList_int_add(ls, 5);

    LList_int_clear(ls);
    LList_int_printNodes(ls);


    printf("last val: %p\n", ls->last->val);
    printf("first val: %p\n", ls->first->val);

    /*
    LList_int_removeAt(ls, 3);
    LList_int_removeAt(ls, 3);
    LList_int_removeAt(ls, 3);

    LList_int_printNodes(ls);

    printf("last val: %i\n", ls->last->val);
    printf("first val: %i\n", ls->first->val);

    LList_int_removeAt(ls, 1);
    printf("last val: %i\n", ls->last->val);
    printf("first val: %i\n", ls->first->val);
    */

    return 0;
}

/*
    Linked List
        - add +
        - removeAt
        - remove
        - first
        - last

        - p len
         
        - clean
        - destroy
*/




