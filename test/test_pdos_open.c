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

    printf("\n");

    // open a file that does not exist within the directory
    PDOS_FILE *file = pdos_open("newfile.txt", "r");
    
    result = system("hexdump -C /dev/shm/TestFS");
    if (result == 0) { printf("Success: File created successfully.\n"); }
    else { printf("Error: File creation failed.\n"); }


    // open a file that does exist within the directory
    // will need to code later once support for creating files is added

    // delete the disk
    free(file);
    remove("/dev/shm/TestFS");

    return 0;
}