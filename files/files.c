
/*
    1. open read ->  stdout
    2. write in file
    3. open, write, out
    4. clean
    5. вписать в конец
    6. вписать в начало
    7. 
*/

#include <stdlib.h>
#include <stdio.h>

int write_end(char* path, char* str) {
    FILE* pt;
    pt = fopen(path, "a+"); 

    if (pt == NULL)
    {
        printf("error with open file. mod: a+");
        return -1;
    } 

    /*
    int i = 0;
    while(str[i] != '\0') {
        fputc(str[i], pt);
        i++;
    }
    */

    fputs(str, pt);

    fclose(pt);

    return 0;
}

int write_begin(char* p, char* str) {
    FILE* pt;
    pt = fopen(p, "w+");

    if (pt != NULL)
    {
        fputs(str, pt);
        fclose(pt);
        return 0;
    }
    else return -1;
}

char* read_offset(char *p, int pos, int len) {
    FILE *pt = fopen(p, "r");
    if (pt == NULL) {
        return 0;
    }

    fseek(pt, (long)pos, SEEK_SET);

    char* dst = (char*)malloc(sizeof(char) * (++len));

    fread((void*)dst, sizeof(char), len, pt);

    fclose(pt);

    dst[len] = '\0';

    return dst;
}

struct Foo {
    int age;
    char* name;
};

struct Foo* init_foo(int age, char* name) {
    struct Foo* pt = (struct Foo*)malloc(sizeof(struct Foo*));

    pt->age = age;
    pt->name = name;

    //printf("qwd");

    return pt;
}

void foo_print(struct Foo* pt) {
    printf("age: %i \n", pt->age);
    printf("name: %s \n", pt->name);
}

int main() {

    printf("sz: %i\n", sizeof(void*));
    printf("sz: %i\n", sizeof(struct Foo*));
    printf("sz: %i\n", sizeof(struct Foo));

    // static
    //struct Foo* pt = malloc(sizeof(struct Foo) * 2);
    struct Foo** pt = malloc(sizeof(struct Foo) * 2 + sizeof(void*));

    printf("sz pt: %i\n", sizeof(pt));

    pt[0] = init_foo(1, "sam");
    pt[1] = init_foo(2, "bob");

    //foo_print(pt[1]);

    FILE* fp = fopen("tmp.bin", "rb+");

    struct Foo** arr = malloc(sizeof(struct Foo) * 2 + sizeof(void*));

    int src[3] = {5, 1, 2};

    printf("%i \n", src[0]);
    printf("%i \n", src[1]);
    printf("%i \n", src[2]);

    size_t sz = sizeof(src[0]);

    int dst[3];

    if (fp != NULL) {

        int written = fwrite(src, sz, 3, fp);

        printf("written: %i \n", written);

        FILE* rp = fopen("tmp.bin", "r");

        int readed = fread(dst, sz, 3, rp);
        printf("rd: %i \n", readed);

        fclose(rp);
    }

    printf("%i \n", dst[0]);
    printf("%i \n", dst[1]);
    printf("%i \n", dst[2]);

    //foo_print(arr[0]);
    //foo_print(arr[1]);


    return 0;
}