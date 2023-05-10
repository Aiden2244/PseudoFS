#include "pdosfilesys.h"

int pdos_fputc(char c, PDOS_FILE *file) {

    // check if the file is in write or read/write mode
    if (strcmp(file->mode, "r") == 0) {
        printf("Error: File is not in write mode.\n");
        return 1;
    }

    // write the character to the buffer
    file->buffer->data[file->pos] = (char) c;

    // increment the position
    file->pos++;

    // if buffer is full, flush the buffer
    if (file->pos == BLOCK_SIZE) {
        pdos_flush(file);
        if (pdos_flush(file) != 0) {
            printf("Error: Could not flush buffer.\n");
            return 1;
        }
    }

    // get the shared memory
    int shm_fd = pdos_get_shm_fd();

    // increment the file size
    DISK_BLOCK *dir_block = pdos_get_disk_block(shm_fd, DIR_BLOCK);
    dir_block->dir.dir_entry_list[file->entrylistIdx].filelength++;
    

    // clean up
    if (pdos_free_disk_block(dir_block) != 0) {
        printf("Error: Could not free disk block.\n");
        return 1;
    }
    close(shm_fd);

    return 0;
}
