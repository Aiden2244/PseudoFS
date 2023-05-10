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
    PDOS_FILE *file = pdos_open("myfile.txt", "r");
    if (file != NULL) {
        printf("Success: File created successfully.\n");
    } else {
        printf("Error: File creation failed.\n");
    }

    result = system("hexdump -C /dev/shm/TestFS");
    if (result == 0) { printf("Disk dump successful.\n"); }
    else { printf("Error: Disk dump failed.\n"); }

    printf("\n");

    // open a second file that does not exist within the directory
    PDOS_FILE *file2 = pdos_open("myfile2.txt", "r");
    if (file2 != NULL) {
        printf("Success: 2nd File created successfully.\n");
    } else {
        printf("Error: 2nd File creation failed.\n");
    }

    result = system("hexdump -C /dev/shm/TestFS");
    if (result == 0) { printf("Disk dump successful.\n"); }
    else { printf("Error: Disk dump failed.\n"); }

    printf("\n");

    // access the file that was created
    PDOS_FILE *file3 = pdos_open("myfile.txt", "r");
    if (file3 != NULL) {
        printf("Success: File accessed successfully.\n");
    } else {
        printf("Error: File access failed.\n");
    }

    result = system("hexdump -C /dev/shm/TestFS");
    if (result == 0) { printf("Disk dump successful.\n"); }
    else { printf("Error: Disk dump failed.\n"); }

    // validate that the fields of file = the fields of file3
    if ((strcmp(file->mode, file3->mode) == 0) 
        && (file->pos == file3->pos) 
        && (file->entrylistIdx == file3->entrylistIdx)
        && (file->blocknum == file3->blocknum)) {
        printf("Success: File fields are equal.\n");
    }
    else {
        printf("Error: File fields are not equal.\n");
    }


    // open a file that does exist within the directory
    // will need to code later once support for creating files is added

    // delete the disk
    pdos_close(file);
    pdos_close(file2);
    pdos_close(file3);

    remove("/dev/shm/TestFS");

    return 0;
}