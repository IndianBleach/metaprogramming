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

 void strclean(char* s, char* in) {
    int c = 0;
    int i = 0;
    
    bool sf = false;
    bool tf = false; 

    while(s[i] != '\0') {
        if (s[i] == ' ' && sf == false) {
            sf= true;
            c++;
        }
        else if (s[i] == '\t' && tf == false) {
            tf = true;
            c++;
        }
        else {
            sf=false;
            tf=false;
            c++;
        }
        i++;
    }

    sf = false;
    tf = false;

    i = 0;
    int t = 0;
    char arr[c];

    c = 0;

    while(s[i] != '\0') {
        if (s[i] == ' ' && sf == false) {
            sf= true;
            arr[c++] = s[i];
        }
        else if (s[i] == '\t' && tf == false) {
            tf = true;
            arr[c++] = s[i];
        }
        else {
            sf=false;
            tf=false;
            arr[c++] = s[i];
        }

        i++;
    }

    arr[c] = '\0';

    in = &arr[0];
}


int main() {
    char* t = "apple\0";
    char* res;
    strclean(t, res);

    printf("%c \n", res[0]);

    int i  =0;
    while((res[i] != '\0')) {
        printf("%c", res[i]);
        i++;
    }

}