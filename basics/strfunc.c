
// squeeze(char[] s, int c) - удаляет с из s[]
// squeeze(char[] s, char[] t) - удаляет t[] из s[]
// strconcat(char[] s, char[] t) - добавляет в конец

// indexOf(char[] src, char[] s) - индекс подстроки в src
// indexOfAny(char[] src, char[] s) - индекс любого символа из s

// replace(char[] src, char[] s, bool all) - замена

// getint

// binsearch
// shell sort

// atof
// atoi

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


bool _isspace(char c) {
    return (c >= '0' && c <= '9') == false && c != '-'; 
}



bool _isdigit(char c) {
    return c >= '0' && c <= '9';
}

double _atof(char s[]) {
    double power;
    int i = 0;
    double val = 0.0;

    while(_isspace(s[i])) {
        i++;
        //printf("ch: %c", s[i]);
    }

    int sign = s[i] == '-' ? -1 : 1;

    if (s[i] == '-' ||
        s[i] == '+') i++;


    while(s[i] >= '0' && s[i] <= '9') {
        
        val = 10.0 * val + (s[i] - '0');
        i++;
        printf("val: %f \n", val);
    }

    if (s[i] == '.') {
        i++;
    }

    for(power = 1.0; _isdigit(s[i]) == true; i++) {
        val = 10.0 * val + (s[i]  - '0');
        power *= 10.0;
    }


    return sign * val / power;
}

int _atoi(char s[]) {
    return (int)(_atof(s));
}


char* squeeze(char src[], int c) {
    bool find = false;
    int i=0;
    int len = 0;
    while(src[i] != '\0') {
        if (src[i] != c) {
            len++;
        }
        i++;
    }

    printf("len: %i", len);

    char* res = malloc(sizeof(char)  * len);
    i = 0;
    int cur = 0;
    while(src[i] != '\0') {
        if (src[i] != c) {
            res[cur++] = src[i]; 
        }
        i++;
    }

    res[cur] = '\0';

    return res;
}

// apple\0 ple\0
char* strsqueeze(char src[], char s[], bool all) {
    int i = 0;
    int eq_index = 0;

    int s_count = 0;
    int s_len = 0;
    bool s_setted = false;

    while(src[i] != '\0') {
        
        if (s[eq_index] != '\0' && src[i] == s[eq_index]) {
            eq_index++;
            if (s_setted == false) {
                s_len++;
            }
        }

        if (s[eq_index] == '\0') {
            s_count++;
            eq_index = 0;
            s_setted = true;
        }
        
        i++;
    }

    int clean_len = i - (s_len * s_count);
    
    char* p = malloc(sizeof(char) * clean_len);
    i = 0;
    eq_index = 0;
    int cur = 0;

    while(src[i] != '\0') {
        if (s[eq_index] != '\0' && src[i] == s[eq_index]) {
            i++;
            eq_index++;
            int win_size = 1;
            bool finded = false;
            while(src[i++] == src[eq_index++]) {
                if (src[i] == '\0') {
                    finded = true;
                }
                win_size++;
            }

            if (!finded) {
                i -= (win_size-1);
                eq_index = 0;
                p[cur++] = src[i];
                //printf("finded: %i \n", i);
                continue;
            }
        }

        printf("char: %c %i\n", src[i], i);

        p[cur++] = src[i];
        i++;
    }

    p[cur] = '\0';

    return p;

    //printf("%i", clean_len);

}

int main() {

    //char* t1 = strsqueeze("appleplepl", "ple", true);

    int* f1;
    getint(f1);


    return 0;
}

