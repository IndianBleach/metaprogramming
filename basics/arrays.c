#include <stdio.h>

#define ARR_LENGHT 10

void printInputGist(int* arr, int size) {
    for(int i = 0; i < size; i++) {
        printf("%d: ", i);
        for(int t = 0; t < arr[i]; t++) {
            printf("==");
        }
        printf("\n");
    }
}

void inputInfo() {
    int c;

    int arr[ARR_LENGHT];
    for(int i =0 ; i <= ARR_LENGHT; i++) {
        arr[i] = 0;
    }

    while((c = getchar()) != EOF) {
        if (c >= '0' && c <= '9') {
            arr[c - '0']++;
        }
        else arr[ARR_LENGHT]++;
    }

    printInputGist(&arr[0], ARR_LENGHT);
}




int main(int argc, char** argv) {
    inputInfo();
    return 0;
}