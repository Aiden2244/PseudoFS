#include <sys/stat.h>
#include "../libPseudoFS/pdosfilesys.h"
#include "../libPseudoFS/pdos_mkdisk.c"

int main(int argc, char *argv[]) {

    // the size of the shared memory object should be 1024 * 1024
    int sz = 1024 * 1024;

    // Create a shared memory object with the name "TestFS"
    pdos_mkdisk("TestFS");

    // Open the shared memory object
    int shm_fd = shm_open("TestFS", O_RDONLY, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Get the size of the shared memory object
    struct stat sb;
    if (fstat(shm_fd, &sb) == -1) {
        perror("fstat");
        close(shm_fd);
        return 1;
    }

    // Check if the size of the shared memory object matches the specified size
    if (sb.st_size == sz) {
        system("hexdump -C /dev/shm/TestFS");
        printf("Success: Shared memory object created with the correct size.\n");
    } else {
        printf("Failure: Shared memory object size does not match the specified size.\n");
    }
    remove("/dev/shm/TestFS");

    // Close the file descriptor
    close(shm_fd);

    return 0;
}
