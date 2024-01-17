#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "heapapi.h"
#include <stdint.h>

void func() {
    int r = 5;
    register int * foo asm ("rsp");
    printf("func %p \n", foo);
}

// check rsp reg to current stack ptr frame
// create macros @func

// let b = &val;
//b = NULL;
// gc.collect
//b = &val; // error' - сравниваем со всеми прошлыми значениями

// на вход даём файл с си кодом (можно этот же файл)


//register int * foo asm ("rsp");


// стек растет вниз по 8 байт на sizeof(void*)
// есть список обьектов на куче черех свой _malloc
// можем отследить стек и вызов функций
// вызов collect после функции очистит корректно
// свойства структур и граф обьектов

// alg
/*
    1. проход по куче, сравнение адресов со стековыми переменными
    2. установить stack_base, stack_size(для окончания скана внутри функций)
    3. загрузить граф обьектов с проверками дочерних элементов
    4. поколения и очередь 0 1 2 для сканирования 
    -> в отдельный поток gc, чтобы работал во время работы программы
*/

/* Problems
    1. обработка темповых NULL 
        (через лист присваиваний внутри фрейма? на выходе из фрейма,
         проверить выходную переменную с листом присваиваний)
    
    2. обработка дочерних элементов в дереве обьектов
    3. обработка циклов
*/

/* Utils
    _list
    _malloc
    _bsearch over addresses

*/

void foo() {
    int* f2 = (int*)malloc(sizeof(int));
    printf("(foo) stack: %p, heap: %p \n", &f2, &(*f2));
}

int* bar() {
    int* f2 = (int*)malloc(sizeof(int));
    printf("(bar) stack: %p, heap: %p \n", &f2, &(*f2));
    return f2;
}

// 000000f386bffc38
// 000000f386bffc30

int main() {

    int* f1 = (int*)malloc(sizeof(int));

    printf("stack: %p, heap: %p \n", &f1, &(*f1));

    foo();

    int* f3 = (int*)malloc(sizeof(int));

    printf("stack: %p, heap: %p \n", &f3, &(*f3));

    int* f4 = (int*)malloc(sizeof(int));

    printf("stack: %p, heap: %p \n", &f4, &(*f4));

    int* f5 = (int*)malloc(sizeof(int));

    printf("stack: %p, heap: %p \n", &f5, &(*f5));

    
    int* f7 = (int*)malloc(sizeof(int));

    printf("(return) stack: %p, heap: %p \n", &f7, &(*f7));
    
    f7 = NULL; // проверить если след обьявленияя не NULL

    printf("(return NULLED) stack: %p, heap: %p \n", &f7, &(*f7));

    int* f6 = (int*)malloc(sizeof(int));

    printf("stack: %p, heap: %p \n", &f6, &(*f6));

    foo();
    bar();

    //printf("%i %i", (void*)000000f386bffc38, sizeof(long*));



    return 0;
}