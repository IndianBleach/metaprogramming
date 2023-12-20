#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "2.3.4"

#define CODE(i) \
printf("%d \n", i);

#define LOG_ERROR(format, ...) \
    fprintf(stderr, format, __VA_ARGS__)

#define LOG_MSG(format, ...) \
    fprintf(stdout, format, __VA_ARGS__);

void print_bytes(void* pt, size_t len) {
    char delim = ' ';
    unsigned int* p = pt;

    for(size_t i = 0; i < len; i++) {
        printf("%c 0x%x", delim, *p);
        delim = ',';
        p++;
    }

    printf("\n");
}

typedef struct {
    char first;
    char sec;
    int count;
} sample_t;

void print_st_bytes(sample_t* s) {
    unsigned char* pt = (unsigned char*)s;
    for(size_t i = 0; i < sizeof(pt); i++, pt++) {
        printf("%d ", (unsigned int)*pt);
    }
}


int main() {
    
    sample_t s;
    s.count = 809;
    s.first =888;
    s.sec = 'v';

    print_st_bytes(&s);

    //printf("%i", sizeof(char));



    return 0;
}