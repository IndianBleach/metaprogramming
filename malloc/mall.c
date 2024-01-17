#include <stdlib.h>
#include <stdio.h> 
#include "heapapi.h"
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

struct Foo {
    int* int_pt;
    char* char_pt;
    long l;
};

struct Foo* init_foo () {
    struct Foo* f = (struct Foo*)malloc(sizeof(struct Foo));
    printf("sz = %i \n", sizeof(struct Foo));
    f->int_pt = (int*)malloc(sizeof(int)); 
    *f->int_pt = 15;
    f->char_pt = (char*)malloc(sizeof(char));
    *f->char_pt = 'd';
    return f;
}
 
void func_2() {
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("func_2 rsp: %p ebp: %p\n", pt, ebp);
    int t1 = 15;
    int r1 = 10;
    //register void* pt __asm__("rsp");

    printf("func_2 (t1) %p \n", &t1);
    printf("func_2 (r1) %p \n", &r1);
}


void func_1() {
    int t1 = 15;
    int r1 = 10; 
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("func_1 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)pt - (void*)ebp);
    printf("func_1 (t1) %p \n", &t1);
    printf("func_1 (r1) %p \n", &r1);
    func_2();
    int t = 5;
    int r = 10;
    printf("func_1 (t2) %p \n", &t);
    printf("func_1 (r2) %p \n", &r);
}

void func_4(int x) {
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("func_4 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
}

// rsp != prevRsp => вложенность функций
// rsp -> ebp frame window
// как только изменяется rsp-ebp

void func_3() {
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("add %p \n", pt+32);
    printf("func_3 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
    func_4(10);
    long t1= 1; //4
    register void* pt2 __asm__("rsp");
    register void* ebp2 __asm__("ebp");
    printf("func_3 rsp: %p ebp: %p, diff %i \n", pt2, ebp2, (void*)ebp2 - (void*)pt2);
}

void bar_1() {
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("bar_1 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
}

// 16 frame

void bar_2() {
    double t = 5;
    double t2 = 5;
    double t3 = 5;
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("bar_2 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
}




void* t1() {
    for(int i = 0; i< 50; i++) {
        bar_1();
    }
}

// проходить по фрейму стека
// как только дошли до конца фрейма, можно добавить фрейм на очистку с указанеим адресов кучи



void* _pt;
void* _ebp;

void bar_3() {
    double* t = (void*)malloc(8);
    double t2 = malloc(8);
    double t3 = malloc(8);
        double t4 = malloc(8);
            double t5 = malloc(8);
                double t6 = malloc(8);
        pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&new_mutex);
        register void* pt __asm__("rsp");
        register void* ebp __asm__("ebp");
        _pt = pt;
        _ebp = ebp;
        pthread_mutex_unlock(&new_mutex);
    printf("bar_3 rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
}

void* gc_scan() {
    void* prev_rsp = NULL;
    void* prev_ebp= NULL;
    while(true) {
        void* pt = _pt;
        void* ebp = _ebp;
        //register void* ebp __asm__("ebp");
        
       //printf("___________(frame) rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);

        if (prev_ebp != ebp || prev_rsp != pt) {
            printf("(frame) rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
            prev_ebp = ebp;
            prev_rsp = pt;
        }

    }
}

void t_1() {

    pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&new_mutex);
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    _pt = pt;
    _ebp = ebp;
    pthread_mutex_unlock(&new_mutex);

    printf("t rsp: %p ebp: %p, diff %i \n", _pt, _ebp, (void*)_ebp - (void*)_pt); 
    //return 0;
}

int main() {

    pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&new_mutex);
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    _pt = pt;
    _ebp = ebp;
    pthread_mutex_unlock(&new_mutex);

    printf("main rsp: %p ebp: %p, diff %i \n", _pt, _ebp, (void*)_ebp - (void*)_pt);

    pthread_t secId;
    pthread_create(&secId, NULL, gc_scan, NULL);
    //pthread_join(secId, NULL);

    sleep(3);

    for(int i = 0; i < 100; i++) {
        t_1(); // -32
    
        //pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&new_mutex);
        register void* pt __asm__("rsp");
        register void* ebp __asm__("ebp");
        _pt = pt;
        _ebp = ebp;
        pthread_mutex_unlock(&new_mutex);
    }

    bar_3();

    sleep(2);

    //printf("main rsp: %p ebp: %p, diff %i \n", _pt, _ebp, (void*)_ebp - (void*)_pt);

    //printf("diff=%p\n", pt-16-32 ); // start at pt-16 by [16] -> 

    return 0;
}


int main2() {
    printf("sz: %i \n", sizeof(double));
    //func_3();
    //printf("%p \n", (void*)__asm__("rsp"));
    
    register void* pt __asm__("rsp");
    register void* ebp __asm__("ebp");
    printf("main rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);

    pthread_t secId;
    pthread_create(&secId, NULL, gc_scan, NULL);
    //pthread_join(secId, NULL);

    sleep(3);

    for(int i = 0; i < 5; i++) {
        bar_1();
        bar_3();
    }

    double f1 = 5;
    double f2 = 5;
    double f3 = 5;

    printf("____end of main\n");

    while(pt <= ebp) {
        printf("-> scan rsp: %p ebp: %p, diff %i \n", pt, ebp, (void*)ebp - (void*)pt);
        pt+=16;
    }

    return 0;
}