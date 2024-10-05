

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

sem_t *sem;

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
    sem_wait(sem);
    int32_t t = *sh_counter;
    t++;
    *sh_counter = t;
    sem_post(sem);
}

void sync_init() {
    sem = sem_open("/sem1", O_CREAT | O_EXCL, 0600, 1);
    if (sem == SEM_FAILED) {
        printf("fail: sem_open\n");
        exit(1);
    }
}

void sync_shutdown() {
    if (sem_close(sem) < 0) {
        printf("fail: sync_shutdown.sem_close\n");
        exit(1);
    }

    if (sem_unlink("/sem1") < 0) {
        printf("fail: sync_shutdown.sem_unlink\n");
        exit(1);
    }
}

// shm_open / open (file)
// shm_unlink / remove (file)

int main() {
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