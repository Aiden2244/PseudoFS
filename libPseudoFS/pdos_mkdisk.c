#include "pdosfilesys.h"

void pdos_mkdisk(const char *fileSystemName) {

    int sz = BLOCK_SIZE * MAXBLOCKS;

    // Create a file in /dev/shm
    char shm_path[256];
    snprintf(shm_path, sizeof(shm_path), "/dev/shm/%s", fileSystemName);
    int fd = open(shm_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return;
    }

    // Set the size of the file
    if (ftruncate(fd, sz) == -1) {
        perror("ftruncate");
        close(fd);
        return;
    }

    // Close the file descriptor
    close(fd);
}
