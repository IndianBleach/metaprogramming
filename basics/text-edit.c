#include "funcs.h"

/*
    left
    clean_comments
    put_to_buff
*/


#define TEXT_BUFF_LIM 500
#define SRC_BUFF_LEN 1000

extern int left_n;
extern char text_buff[];
extern const int BUFF_LEN;

char src_buff[SRC_BUFF_LEN];


void left(int n) {

    int i = 0;

    int c;
    int pos = 0;


    while((c = getchar()) != EOF && i < TEXT_BUFF_LIM) {
        if (c == '\n') {
            pos = 0;
            text_buff[i++] = c;
        }
        else if (pos == n) {
            text_buff[i++] = '\n';

            if (c != ' ') {
                text_buff[i++] = c;
            }
            
            pos = 0;
        }
        else {
            text_buff[i++] = c;
            pos++;
        }
    }

    text_buff[i] = '\0';
}

// right

void clean_comments(char arr[], int size) {
    int i = 0;
    int pos = 0;
    while(arr[i] != EOF && i < size) {
        if (arr[i] == '/' && arr[i+1] == '/') {
            while(arr[i++] != '\n') {
            }
            i--;
        }
        else {
            text_buff[pos++] = arr[i];
        }
        i++;
    }
} 

int put_to_buff() {
    int c;
    int pos = 0;

    while((c = getchar()) != EOF && pos < BUFF_LEN) {
        src_buff[pos++] = c;
    }

    return pos;
}

int main(int argc, char* argv) {
    
    int len = put_to_buff();

    clean_comments(src_buff, len);

    printf("%s", text_buff);
    
    //printf("%s", text_buff);
}