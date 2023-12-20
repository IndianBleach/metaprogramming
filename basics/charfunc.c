
// getch
// ungetch

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFF_LEN 1000
char buff[BUFF_LEN];
int  buff_p = 0;

#define TYPE_INT int
#define TYPE_CHAR char

#define swap(t, x, b)\
    t temp = b;\
    b = x;\
    x = temp;

int getch(void) {
    return buff_p > 0  ? buff[--buff_p] : getchar();
}

void ungetch(int c) {
    if (buff_p < BUFF_LEN) {
        buff[buff_p++] = c;
    }
}

int _isdigit_fromint(int c) {
    bool f = c >= '0' && c <= '9';
    return f == true ? 1 : 0;
}

bool _isspace(int c) {
    return (c >= '0' && c <= '9' ) == false;
}

int getint(int* in) {
    // skip spaces
    int c;
    while(_isspace(c = getch())) {        
    }

    int sign = 1;
    // check sign 
    if (c == '-') {
        sign = -1;
    }

    if (c == '-' || c == '+') {
        c = getch();
    }

    int flag = _isdigit_fromint(c);
    printf("flag: %i \n", flag);

    *in = 0;
    while( _isdigit_fromint(c) == 1) {
        *in = *in * 10 + (c - '0');
        c = getch();
    }

    *in *= sign;

    return c;
}

bool strequal(char* p, char* t) {
    int i = 0;
    while(p[i] == t[i] != '\0') {
        i++;
    }

    if (p[i] == '\0') return true;

    return false;
}





int main(int argc, char *argv[]) {
    
    int* p = (int*)malloc(sizeof(int));
    getint(p);

    printf("result: %i \n", *p);

    return 0;
}