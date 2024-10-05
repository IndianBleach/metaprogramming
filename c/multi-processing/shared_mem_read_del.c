

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


#include <sys/mman.h>

int main() {
    int shrd_mem_size = 16;

    // create/get shared_memory
    int shm_fd = shm_open("/shm0", O_RDONLY, 0600);
    if (shm_fd < 0) {
        printf("fail: shm_open\n");
        return;
    }

    // map memory to this proccess.
    void *map = mmap(0, shrd_mem_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (map == MAP_FAILED) {
        printf("fail: mmap\n");
        return;
    }

    // casting
    char *buff = (char *) map;
    printf("buff=%s\n", buff);

    // unmap
    if (munmap(buff, shrd_mem_size) < 0) {
        printf("fail: munmap\n");
        return;
    }

    // close fd
    if (close(shm_fd) < 0) {
        printf("fail: close\n");
        return;
    }

    // unlink shared_memory
    if (shm_unlink("/shm0") < 0) {
        printf("fail: shm_unlink\n");
        return;
    }

    return 0;
}