
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mtx;

void *thread_body(void *arg) {
    int *cur = (int *) arg;

    pthread_mutex_lock(&mtx);

    (*cur)++;
    printf("_1=%i\n", *cur);

    pthread_mutex_unlock(&mtx);

    return NULL;
}

void *thread_body2(void *arg) {
    int *cur = (int *) arg;

    pthread_mutex_lock(&mtx);

    (*cur) += 2;
    printf("_1=%i\n", *cur);

    pthread_mutex_unlock(&mtx);


    return NULL;
}

// join
// detached threads


int main() {
    int i = 0;

    pthread_t tid;
    int res1 = pthread_create(&tid, NULL, thread_body, &i);

    pthread_t tid2;
    int res2 = pthread_create(&tid2, NULL, thread_body2, &i);

    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);

    printf("threads.Main\n");

    pthread_t cur = pthread_self();
    printf("cur2==%i\n", cur);

    return 0;
}