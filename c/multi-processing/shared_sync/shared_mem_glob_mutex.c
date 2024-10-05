

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <pthread.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
// join
// detached threads

// pstree

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

int glob_mutex_fd = -1;
pthread_mutex_t *glob_mutex = NULL;

int shrd_mem_size = sizeof(int32_t);
int shm_fd = -1;
int32_t *sh_counter = NULL;

void shmem_init() {
    shm_fd = shm_open("/shm2", O_CREAT | O_RDWR, 0600);
    printf("shmem_init.shm_open\n");
}

void shmem_shutdown() {
    shm_unlink("/shm2");
    printf("shmem_shutdown.shm_unlink\n");
}

void inc_counter() {
    pthread_mutex_lock(glob_mutex);

    int32_t t = *sh_counter;
    t++;
    *sh_counter = t;

    pthread_mutex_unlock(glob_mutex);
}

void sync_init() {
    // create/get mutex shared mem
    glob_mutex_fd = shm_open("/mutex1", O_CREAT | O_RDWR, 0600);
    if (glob_mutex_fd < 0) {
        printf("sync_init.shm_open\n");
        exit(1);
    }

    // truncate mutex mem
    if (ftruncate(glob_mutex_fd, sizeof(pthread_mutex_t)) < 0) {
        printf("fail: sync_init.ftruncate\n");
        exit(1);
    }

    // mapped mutex memory
    void *map = mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, glob_mutex_fd, 0);

    glob_mutex = (pthread_mutex_t *) map;

    // init mutex
    int ret = -1;
    pthread_mutexattr_t attr;
    if ((ret = pthread_mutexattr_init(&attr))) {
        printf("fail: sync_init.pthread_mutexattr_init\n");
        exit(1);
    }

    // set mutex shared mem
    if ((ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED))) {
        printf("fail: sync_init.pthread_mutexattr_setpshared\n");
        exit(1);
    }

    // init mutwex
    if ((ret = pthread_mutex_init(glob_mutex, &attr))) {
        printf("fail: sync_init.pthread_mutex_init\n");
        exit(1);
    }

    // destr attr
    if ((ret = pthread_mutexattr_destroy(&attr))) {
        printf("fail: sync_init.pthread_mutexattr_destroy\n");
        exit(1);
    }
}

void sync_shutdown() {
    int ret = -1;

    // destr local mutex
    if ((ret = pthread_mutex_destroy(glob_mutex))) {
        printf("fail: sync_shutdown.pthread_mutex_destroy\n");
        exit(1);
    }

    // unmap shared mutex memory
    if ((ret = munmap(glob_mutex, sizeof(pthread_mutex_t))) < 0) {
        printf("fail: sync_shutdown.munmap\n");
        exit(1);
    }

    // close mutex_fd
    if ((ret = close(glob_mutex_fd)) < 0) {
        printf("fail: sync_shutdown.close\n");
        exit(1);
    }

    // unlink mutex shared memory
    if ((ret = shm_unlink("/mutex1")) < 0) {
        printf("fail: sync_shutdown.shm_unlink\n");
        exit(1);
    }
}

// shm_open / open (file)
// shm_unlink / remove (file)

int main() {
    printf("[sync.shared_mutex]\n");

    // create shd_mem
    shmem_init();

    sync_init();

    // truncate
    ftruncate(shm_fd, shrd_mem_size);
    printf("main.ftruncate\n");

    // mmap
    void *map = mmap(0, shrd_mem_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // set init val
    sh_counter = map;
    *sh_counter = 0;

    pid_t pid = fork();
    if (pid) {
        // parent process.
        inc_counter();
        printf("PAR=%i\n", *sh_counter);

        int status = -1;
        wait(&status);
        printf("child process finished.\n");
    } else {
        // child process
        inc_counter();
        printf("CHILD=%i\n", *sh_counter);
    }

    // shutdown TWICE shared_memory
    munmap(sh_counter, shrd_mem_size);
    close(shm_fd);

    // only parent can UNLINK the shared_mem
    if (pid) {
        sync_shutdown();
        shmem_shutdown();
    }

    // fork()
    // write to mem

    return 0;
}