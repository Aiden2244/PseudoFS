#include "pdosfilesys.h"

int pdos_free_disk_block(DISK_BLOCK* block, int block_num) {
    // calculate the page number
    int offset_within_page = block_num % 4;

    // adjust the pointer 
    DISK_BLOCK *page = block - offset_within_page;

    // free the page
    munmap(page, PAGE_SIZE);

    return 0;
}