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

    // open a file that does not exist within the directory
    PDOS_FILE *file = pdos_open("isNotReal.txt", "r");

    // open a file that does exist within the directory
    // will need to code later once support for creating files is added

    // delete the disk
    remove("/dev/shm/TestFS");

    return 0;
}