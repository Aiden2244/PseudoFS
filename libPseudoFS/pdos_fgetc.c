#include "pdosfilesys.h"

char pdos_fgetc(PDOS_FILE *file) {
    // check if the file is in read or read/write mode
    if (strcmp(file->mode, "w") == 0) {
        printf("Error: File is not in read mode.\n");
        return EOF;
    }

    int shm_fd = pdos_get_shm_fd();

    // if the file buffer is empty, read the first block in the chain
    if (file->buffer == NULL && file->pos == 0) {
        DISK_BLOCK *dir_block = pdos_get_disk_block(shm_fd, DIR_BLOCK);
        if (dir_block == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            return EOF;
        }
        int block_num = dir_block->dir.dir_entry_list[file->entrylistIdx].filefirstblock;
        pdos_free_disk_block(DIR_BLOCK);

        file->buffer = pdos_get_disk_block(shm_fd, block_num);
        if (file->buffer == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            return EOF;
        }

        file->blocknum = block_num;
    }

    // if we've read all data in the current block, get the next block from the disk
    if (file->pos == BLOCK_SIZE) {
        // Load FAT and get the next block number
        DISK_BLOCK *fat_block = pdos_get_disk_block(shm_fd, FAT_TABLE);
        if (fat_block == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            return EOF;
        }
        int next_block = fat_block->fat[file->blocknum];
        pdos_free_disk_block(FAT_TABLE);

        // Check if there is no next block
        if (next_block == ENDOFCHAIN) {
            return EOF;
        }

        // Free the current block
        pdos_free_disk_block(file->blocknum);

        // Load the next block into the buffer
        file->buffer = pdos_get_disk_block(shm_fd, next_block);
        if (file->buffer == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            return EOF;
        }

        file->blocknum = next_block;
        file->pos = 0;
    }

    // read the character from the buffer
    char c = file->buffer->data[file->pos];

    // increment the position
    file->pos++;

    close(shm_fd);

    return c;
}



