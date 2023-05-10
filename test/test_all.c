#include "../libPseudoFS/pdosfilesys.h"
#include <string.h>
#include <ctype.h>

int main() {
    // Create the disk
    pdos_set_disk_name_var("TestFS");
    pdos_mkdisk(1024 * 1024);

    // Create the file system
    pdos_mkfs("Test File System");

    // initial state of file system
    printf("\n");
    printf("Initial state of file system:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Open a file in write mode
    PDOS_FILE *file = pdos_open("myfile.txt", "w");
    if (file != NULL) {
        printf("Success: File created successfully.\n");
    } else {
        printf("Error: File creation failed.\n");
        remove("/dev/shm/TestFS");
        return -1;
    }

    printf("\n");
    printf("Opened file myfile.txt in write mode:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Write some data to the file
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        if (pdos_fputc(ch, file) == EOF) {
            printf("Error: Failed to write character '%c' to file.\n", ch);
            remove("/dev/shm/TestFS");
            return -1;
        }
    }

    printf("\n");
    printf("Wrote data to myfile.txt:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Close the file (which should call pdos_flush)
    if (pdos_close(file) == EOF) {
        printf("Error: Failed to close file.\n");
        remove("/dev/shm/TestFS");
        return -1;
    }

    printf("\n");
    printf("closed myfile.txt:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Open the file again in read mode
    PDOS_FILE *file2 = pdos_open("myfile.txt", "r");
    if (file2 != NULL) {
        printf("Success: File opened successfully for reading.\n");
    } else {
        printf("Error: Failed to open file for reading.\n");
        remove("/dev/shm/TestFS");
        return -1;
    }

    printf("\n");
    printf("Opened file myfile.txt in read mode:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Read the data from the file and verify it
    for (char expected_ch = 'A'; expected_ch <= 'Z'; expected_ch++) {
        int ch = pdos_fgetc(file2);
        if (ch == EOF) {
            printf("Error: Failed to read character from file.\n");
            remove("/dev/shm/TestFS");
            return -1;
        }
        if (ch != expected_ch) {
            printf("Error: Mismatch on read - expected '%c', got '%c'.\n", expected_ch, ch);
            remove("/dev/shm/TestFS");
            return -1;
        }
    }

    printf("\n");
    printf("Verified data is correct in myfile.txt:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Close the file
    if (pdos_close(file2) == EOF) {
        printf("Error: Failed to close file.\n");
        return -1;
    }

    printf("\n");
    printf("close myfile.txt again:\n");
    system("hexdump -C /dev/shm/TestFS");

    // Delete the disk
    remove("/dev/shm/TestFS");

    printf("Success: All tests passed.\n");
    return 0;
}
