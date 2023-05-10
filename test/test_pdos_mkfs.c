#include "../libPseudoFS/pdosfilesys.h"
#include <string.h>

int main() {
    // Create the disk
    pdos_set_disk_name_var("TestFS");
    pdos_mkdisk(1024 * 1024);

    // Create the file system
    pdos_mkfs("Test File System");

    int result = system("hexdump -C /dev/shm/TestFS");

    // if no problems, print success message. Else, print error message.
    if (result == 0) { printf("Success: File system created successfully.\n"); }
    else { printf("Error: File system creation failed.\n"); }

    // delete the disk
    remove("/dev/shm/TestFS");

    return 0;
}