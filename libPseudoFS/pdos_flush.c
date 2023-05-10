#include "pdosfilesys.h"

int pdos_flush(PDOS_FILE *file) {
    // get the shared memory object
    int shm_fd = pdos_get_shm_fd();

    DISK_BLOCK *current_block = pdos_get_disk_block(shm_fd, file->blocknum);

    // write buffer to disk
    memcpy((char*)current_block, file->buffer, BLOCK_SIZE);

    // clear the buffer
    free(file->buffer);
    file->buffer = malloc(BLOCK_SIZE);

    // unmap the block's page
    if (pdos_free_disk_block(current_block) != 0) {
        printf("Error: Could not free disk block.\n");
        return EOF;
    }

    // if the file position is at the end of the block, then we need a new block
    if (file->pos == BLOCK_SIZE) {
        // calculate the page number and offset within the page for the FAT
        int fat_page_number = 1 / 4;
        int fat_offset_within_page = 1 % 4;

        // map the FAT's page into memory
        DISK_BLOCK *fat_page = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, fat_page_number * PAGE_SIZE);
        if (fat_page == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            return EOF;
        }

        // calculate the pointer to the FAT block within the mapped page
        DISK_BLOCK *fat_block = pdos_get_disk_block(shm_fd, FAT_TABLE);

        // find next free block in the FAT
        int free_block = -1;
        for (int i = 0; i < MAXBLOCKS; i++) {
            if (fat_block->fat[i] == UNUSED) {
                free_block = i;
                break;
            }
        }

        // if no free blocks, the disk is full
        if (free_block == -1) {
            printf("Error: Disk is full.\n");
            pdos_free_disk_block(fat_block);
            close(shm_fd);
            return EOF;
        }

        // update the FAT
        fat_block->fat[file->blocknum] = free_block;
        fat_block->fat[free_block] = ENDOFCHAIN;

        // unmap the FAT's page
        if (pdos_free_disk_block(fat_block) != 0) {
            printf("Error: Could not free disk block.\n");
            return EOF;
        }

        // update the file descriptor
        file->blocknum = free_block;

        file->pos = 0;
    }

    if (close(shm_fd) == -1) {
        perror("close");
        return EOF;
    }

    return 0;
}
