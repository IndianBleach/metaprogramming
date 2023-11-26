#include <stdio.h>

#define TEST 10

/*
    files/symbols
    - putchar - посылает символ в стандартный поток вывода, двигает каретку
    - getchar - получает символ с потока ввода
    - getline(cpp) - читает строку из входного потока с делиметром 

*/

void printCelsius() {
    int f, c = 0;
    int i = 1;
    while(i <= 10)
    {
        c+= 10;
        f+= 22;
        printf("celsius: %d\nfahr: %d\n", c, f);

        i++;
    }

    for(int t = 0; t < TEST; t+= 5)
    {
        printf("for\t");
    }
}

void copyInput() {
    int c;
    while((c = getchar()) != EOF) 
        putchar(c); 
}

void inputInfo() {
    int sp = 0, other = 0, tabs = 0;
    int c;
    while((c = getchar()) != EOF) {
        if (c == '\t') {
            tabs++;
        } else if (c == ' ') {
            sp++;
        } else other++;
    }

    printf("tabs: %d, spaces: %d, other: %d", tabs, sp, other);
}

void input_sample2(){
    int c;
    while((c = getchar()) != EOF) {
        if (c == ' ') {
            putchar('\n');
        } else putchar(c);
    }
}

int main()
{
    input_sample2();
}
