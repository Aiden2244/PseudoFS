#include "pdosfilesys.h"

DISK_BLOCK *pdos_get_disk_block(int shm_fd, int block_num) {

    // calculate the page number and offet of the block
    int page_number = block_num / 4;
    int offset_within_page = block_num % 4;

    // we need to read the entire admin block into memory since mmap must be page aligned
    DISK_BLOCK *page = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, page_number * PAGE_SIZE);
    if (page == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(1);
    }

    // use pointer arithmetic to get to the specific block
    DISK_BLOCK *block = (DISK_BLOCK *) ((char *) page + (offset_within_page * BLOCK_SIZE));

    return block;
}