#include "pdosfilesys.h"

char pdos_fgetc(PDOS_FILE *file) {
    // check if the file is in read or read/write mode
    if (strcmp(file->mode, "w") == 0) {
        printf("Error: File is not in read mode.\n");
        return EOF;
    }

    int shm_fd = pdos_get_shm_fd();

    // if the file buffer is empty, read the first block in the chain
    if (file->pos == 0) {

        // Load the first block in the chain
        DISK_BLOCK *dir = pdos_get_disk_block(shm_fd, DIR_BLOCK);

        // Get the block number of the first block in the chain
        int block_num = dir->dir.dir_entry_list[file->entrylistIdx].filefirstblock;
        printf("Block num %d\n", block_num);
        pdos_free_disk_block(dir, DIR_BLOCK);

        DISK_BLOCK *disk_block = pdos_get_disk_block(shm_fd, block_num);
        if (disk_block == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            free(file->buffer);
            file->buffer = NULL;
            return EOF;
        }
        memcpy(file->buffer->data, disk_block, BLOCK_SIZE);
        file->pos = 0;
        pdos_free_disk_block(disk_block, block_num);
        

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
        pdos_free_disk_block(fat_block, FAT_TABLE);

        // Check if there is no next block
        if (next_block == ENDOFCHAIN) {
            return EOF;
        }

        // Free the current block
        free(file->buffer);
        file->buffer = malloc(BLOCK_SIZE);

        // Load the next block into the buffer
        DISK_BLOCK *db = pdos_get_disk_block(shm_fd, next_block);
        if (db == MAP_FAILED) {
            printf("Error: Could not read block from disk.\n");
            return EOF;
        }

        memcpy(file->buffer->data, db, BLOCK_SIZE);
        pdos_free_disk_block(db, next_block);

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



