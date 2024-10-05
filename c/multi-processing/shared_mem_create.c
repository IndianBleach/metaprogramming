

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
    int i = 0;

    // create shared memory
    int shm_fd = shm_open("/shm0", O_CREAT | O_RDWR, 0600);
    if (shm_fd < 0) {
        printf("error: failed to create sharwed memory\n");
        return;
    }

    printf("shm created.\n");


    // set shm size
    int shm_size = 16;
    if (ftruncate(shm_fd, shm_size * sizeof(char)) < 0) {
        printf("shm set memory error\n");
        return;
    }

    printf("shm region truncated\n");

    // mapping
    void *map = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (map == MAP_FAILED) {
        printf("error: mmap shm\n");
        return;
    }

    char *buff = (char *) map;
    strncpy(buff, "hello!", strlen("hello!"));

    printf("set buff=%s\n", buff);

    while (1)
        ;

    if (munmap(buff, shm_size) < 0) {
        printf("error: unmap failed\n");
        return 1;
    }

    if (close(shm_fd) < 0) {
        printf("error: close fd failed\n");
        return 1;
    }

    return 0;
}