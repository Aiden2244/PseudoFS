#include "pdosfilesys.h"

int pdos_mkdisk(int sz) {
    // Create a file in /dev/shm
    int fd = shm_open(disk_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Set the size of the file
    if (ftruncate(fd, sz) == -1) {
        perror("ftruncate");
        close(fd);
        return 1;
    }

    // Close the file descriptor
    close(fd);
    
    return 0;
}
