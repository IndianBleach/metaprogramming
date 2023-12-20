#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
    copy
    getline
    strclean
    rvrse
    reverseLines
    detab
    entab
    
*/

int left_n = 10; 

char buff[1000];

char cur[100];
int size_lim = 100; 

int getline(char arr[], int size) {
    int i = 0;
    int c;

    while((c = getchar()) != EOF && c != '\n' && i < size)
    {
        arr[i++] = c; 
    }

    arr[i] = '\0';

    return i;
}

void copyLong(char from[],  char to[]) {
    int i =0;
    while( (to[i] = from[i]) != '\0') {
        i++;
    }
}

char* strclean(char str[]) {
    int i = 0;

    bool sp_f = false;
    bool tab_f = false; 

    int cool = 0;

    while(str[i] != '\0') {
        if (str[i] == ' ') {
            if (sp_f == false) {
                sp_f = true;
                cool++;
            }
        }
        else if (str[i] == '\t') {
            if (tab_f == false) {
                cool++;
                tab_f = true;
            }
        }
        else {
            tab_f = false;
            sp_f = false;
            cool++;
        }

        i++;
    }

    char* res = malloc(sizeof(char) * cool);

    i = 0;
    sp_f = false;
    int co = 0;
    while( str[i] != '\0') {
        
        if (str[i] != ' ' && str[i] != '\t') {
            res[co++] = str[i];
            sp_f = false;
        }
        else if (sp_f == false && co != cool-1) {
            res[co++] = str[i];
            sp_f = true;
        }
        i++;
    }

    res[co] = '\0';

    return res;
}

char* rvrse(char arr[], int size) {
    char* rv = malloc(sizeof(char) * (size));
    int i = 0;
    for(int i = 0; i < size; i++) {
        rv[i] = arr[size-i-1];
    }

    rv[size] = '\0';

    return rv; 
}

#define out_buff_len 1000
char out_buff[out_buff_len];
int pos = 0;

int outbuff_addLine(char* arr) {
    int i = 0;
    while(arr[i] != '\0' && pos < out_buff_len) {
        out_buff[pos] = arr[i];
        i++;
        pos++;
    }

    out_buff[pos++] = '\n';
}

void reverseLines()
{
    extern char cur[];
    extern int size_lim;
    //extern char out_buff[];
    int i = 0;
    while((i = getline(cur, size_lim)) > 0) {
        outbuff_addLine(rvrse(cur, i));
    }

    printf("%s", out_buff);
}

int tab_len = 4;

char* detab(char arr[]) {
    int i = 0;

    extern int tab_len;
    int len = 0;

    while(arr[i] != '\0') {
        if (arr[i] == '\t') {
            len += tab_len;
            
        }
        else len++;
        i++;
    }

    char* res = malloc(sizeof(char) * (len+1));
   
    int pos = 0;
    i = 0;

    while(arr[i] != '\0') {
        if (arr[i] == '\t') {
            for(int c = 0; c < tab_len; c++) {
                res[pos++] = ' ';
            }
        }
        else {
            res[pos++] = arr[i];
        }
        i++;
    }

    res[pos] = '\0';

    return res;
} 

// заменить пробелы на табы по минимуму 
char* entab(char arr[]) {
    int len = 0;
    int cur = 0;
    int i = 0;

    int sp_window = 0;

    int count_tabs = 0;

    while(arr[i] != '\0') {
        if (arr[i] == ' ') {

            sp_window++;

            if (sp_window == tab_len) {
                count_tabs++;
                sp_window = 0;
            }
        }
        else {
            sp_window = 0;
        }
        i++;
    }
}