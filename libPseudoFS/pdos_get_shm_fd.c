#include "pdosfilesys.h"

int pdos_get_shm_fd() {
    // open the shared memory file descriptor
    int shm_fd = shm_open(disk_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    return shm_fd;
}