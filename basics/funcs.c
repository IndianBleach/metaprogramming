#include <stdio.h>
#include <stdbool.h>


#define BUFF_LEN 1000

char buff[BUFF_LEN]; 

int longest_length = 0; 
char longest[BUFF_LEN];

void reset(char* s) {
    int i = 0;
    longest_length = 0;
    while( s[i] != '\0') {
        longest[i] = s[i];
        longest_length++;
        i++;
    }
}

void printLongest() {
    for(int i =0; i < longest_length; i++) {
        putchar( longest[i]);
    }
}

void printLongestString() {
    int c;
    int cur = 0;
    int i = 0;
    while((c = getchar()) != EOF) {
        buff[i++] = c;
        cur++;
        if (c == '\n') {
            printf("\n %d - %d \n" , cur, longest_length);
            if (cur > longest_length) {
                buff[i] = '\0';
                reset(&buff[0]);
            }
            cur = 0;
            i = 0;
        }
    }

    printLongest();
}


void foo(char* out) {
    char w[] = "same short";
    out = "same";
}

int main() {
    char t[6] = "apple\0";
    char* res = &t[0];
    //strclean(&t[0], res);

    char w[] = "apple world";

    printf("%s \n", res);
    foo(res);
    printf("%s \n", res);
}