#include "../libPseudoFS/pdosfilesys.h"
#include <string.h>
#include <ctype.h>

int main() {
    // Create the disk
    pdos_set_disk_name_var("TestFS");
    pdos_mkdisk(1024 * 1024);

    // Create the file system
    pdos_mkfs("Test File System");

    // Open a file in write mode
    PDOS_FILE *file = pdos_open("myfile.txt", "w");
    if (file != NULL) {
        printf("Success: File created successfully.\n");
    } else {
        printf("Error: File creation failed.\n");
        return -1;
    }

    // Write some data to the file
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        if (pdos_fputc(ch, file) == EOF) {
            printf("Error: Failed to write character '%c' to file.\n", ch);
            return -1;
        }
    }

    // Close the file (which should call pdos_flush)
    if (pdos_close(file) == EOF) {
        printf("Error: Failed to close file.\n");
        return -1;
    }

    // Open the file again in read mode
    file = pdos_open("myfile.txt", "r");
    if (file != NULL) {
        printf("Success: File opened successfully for reading.\n");
    } else {
        printf("Error: Failed to open file for reading.\n");
        return -1;
    }

    // Read the data from the file and verify it
    for (char expected_ch = 'A'; expected_ch <= 'Z'; expected_ch++) {
        int ch = pdos_fgetc(file);
        if (ch == EOF) {
            printf("Error: Failed to read character from file.\n");
            return -1;
        }
        if (ch != expected_ch) {
            printf("Error: Mismatch on read - expected '%c', got '%c'.\n", expected_ch, ch);
            return -1;
        }
    }

    // Close the file
    if (pdos_close(file) == EOF) {
        printf("Error: Failed to close file.\n");
        return -1;
    }

    // Delete the disk
    remove("/dev/shm/TestFS");

    printf("Success: All tests passed.\n");
    return 0;
}
