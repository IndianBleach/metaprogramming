#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define STACK_LEN 100

struct Stack {
    int size;
    char* buff;
    int cur;

    char (*pop)(struct Stack*);
    int (*push)(struct Stack*, char);
    char (*peek)(const struct Stack*);
    void (*clear)(struct Stack*);
    bool (*is_full)(const struct Stack*);
};

bool st_is_full(const struct Stack* st) {
    return st->cur == st->size;
}

int st_push(struct Stack* st, char elem) {
    
    if (st->is_full(st) == 0) {
        st->buff[st->cur++] = elem;
        return 0;
    }
    else return 1;
}

char st_peek(const struct Stack* st) {
    
    if (st->cur == 0) {
        return 0;
    }
    else {
        return st->buff[st->cur-1];
    }
}

void st_clear(struct Stack* st) {

    for(int i = 0; i < st->cur; i++) {
        st->buff[i] = 0;
    }
    st->cur = 0;
}

char pop(struct Stack* st) {
    if (st->cur == 0) {
        return 0;
    }
    else {
        
        char t = st->buff[st->cur-1];

        st->buff[st->cur-1] = 0;
        st->cur--;

        return t;
    }
}

struct Stack* stack_init(int size) {
    char* arr = (char*)malloc(sizeof(char) * size);
     
    arr[size] = '\0';

    struct Stack* p = (struct Stack*)malloc(sizeof(struct Stack));

    p->size = size;
    p->buff = arr;
    p->cur = 0;

    p->push = st_push;
    p->is_full = st_is_full;
    p->peek = st_peek;
    p->clear = st_clear;
    p->pop = pop;

    return p;
}

int main() {

    struct Stack* st = stack_init(5);

    st->push(st, 'a');
    st->push(st, 'p');
    st->push(st, 'p');
    st->push(st, 'l');
    st->push(st, 'e');

    printf("%s \n", st->buff);

    st->clear(st);

    st->push(st, 't');
    st->push(st, 'e');

    char s = st->pop(st);

    printf("%s\n", st->buff);

    st->push(st, 's');
    st->push(st, 't');

    printf("%i\n", st->cur);

    printf("%s\n", st->buff);

    printf("%c\n", s);
}

