#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// _is_full
// _shift
// clear
// enqueue
// dequeue
// contains

struct Queue {
    int size;
    char* buff;
    int last_pos;

    void (*clear)(struct Queue*);
    void (*enqueue)(const struct Queue*, char);
};

void qu_clear(struct Queue* q) {
    for(int i = 0; i < q->size; i++) {
        q->buff[i] = 0;
    }
    q->last_pos = 0;
    q->buff[q->size] = '\0';
}

inline bool qu_is_full(const struct Queue* q) {
    return q->buff[q->size-1] != 0;
}

void qu_shift(struct Queue* q) {
    for(int i = q->size; i > 0; i--) {
        q->buff[i] = q->buff[i-1];
    }

    q->buff[0] = '_';
}

void qu_enqueue(struct Queue* q, char c) {
    qu_shift(q);
    q->buff[0] = c;
    if (q->last_pos < q->size-1) {
        q->last_pos++;
    }
}

char qu_peek(const struct Queue* q) {
    if (q->last_pos == 0) {
        return 0;
    }
    else {
        return q->buff[q->last_pos-1];
    }
}

char qu_dequeue(struct Queue* q) {
    
    char* p = (char*)malloc(sizeof(char*));
    p[0] = qu_peek(q);
    printf("%c \n", p[0]);
    printf("%i \n", q->last_pos);
    q->buff[q->last_pos] = 0;
    
    if (q->last_pos > 0) {
        q->last_pos--;
    }

    return p[0];
}

bool qu_contains(const struct Queue* q, char sym) {
    for(int i = 0; i < q->last_pos+1; i++) {
        if (q->buff[i] == sym) {
            return true;
        }
    }

    return false;
}

struct Queue* Queue(int size) {
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue*));

    char* qbuff = malloc(sizeof(char) * size);

    q->size = size;
    q->buff = qbuff;
    q->last_pos = 0;

    q->clear = qu_clear;


    q->clear(q);

    return q;
}

int main() {
    struct Queue* q = Queue(5);

    qu_enqueue(q, 'a');
    qu_enqueue(q, 't');
    //qu_shift(q);
    char s = qu_dequeue(q);

    char peek = qu_peek(q);
    char s2 = qu_dequeue(q);
    char p2 = qu_peek(q);

    printf("deq: %c \n", s);
    printf("peek: %c \n", peek);
    printf("deq2: %c \n", s2);
    printf("peek2: %c \n", p2);

    qu_enqueue(q, 'a');
    qu_enqueue(q, 'p');
    qu_enqueue(q, 'y');

    bool f1 = qu_contains(q, 'y');
    bool f2 = qu_contains(q, 't');

    printf("f1: %i \n", f1);
    printf("f2: %i \n", f2);

    //qu_enqueue(q, 'p');
    //qu_enqueue(q, 'p');
    printf("%s \n", q->buff);

    //printf("%c \n", q->buff[q->last_pos]);
    //printf("%s \n", q->buff);
    //
}