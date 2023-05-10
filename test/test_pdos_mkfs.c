#include "../libPseudoFS/pdosfilesys.h"
#include "../libPseudoFS/pdos_mkdisk.c"
#include "../libPseudoFS/pdos_mkfs.c"
#include <string.h>

int main() {
    // Create the disk
    pdos_mkdisk("TestFS");

    // Create the file system
    pdos_mkfs("TestFS", "Test File System");

    int result = system("hexdump -C /dev/shm/TestFS");

    // if no problems, print success message. Else, print error message.
    if (result == 0) { printf("Success: File system created successfully.\n"); }
    else { printf("Error: File system creation failed.\n"); }

    // delete the disk
    remove("/dev/shm/TestFS");

    return 0;
}