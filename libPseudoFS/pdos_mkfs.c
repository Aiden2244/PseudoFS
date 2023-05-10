#include "pdosfilesys.h"

void setup_ID_block(char *ID) {
    int shm_fd = shm_open(disk_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, MAXBLOCKS * BLOCK_SIZE) == -1) {
        perror("ftruncate");
        close(shm_fd);
        exit(1);
    }

    // Initialize the zeroth block with the ID
    DISK_BLOCK *block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 * PAGE_SIZE);
    strncpy(block->data, ID, BLOCK_SIZE);
    munmap(block, BLOCK_SIZE);
    close(shm_fd);
}

void setup_FAT_blocks() {
    // Open the shared memory object
    int shm_fd = shm_open(disk_name, O_RDWR, S_IRUSR | S_IWUSR);
    DISK_BLOCK *block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 * PAGE_SIZE);
    if (block == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(1);
    }

    // use pointer arithmetic to get to the second block
    block = (DISK_BLOCK *) ((char *) block + BLOCK_SIZE);

    // Initialize the FAT block
    block->fat[0] = ENDOFCHAIN;
    block->fat[1] = 2;
    block->fat[2] = ENDOFCHAIN;
    block->fat[3] = ENDOFCHAIN;
    block->fat[4] = ENDOFCHAIN;
    for (int i = 5; i < MAXBLOCKS; i++) {
        block->fat[i] = UNUSED;
    }
    munmap(block, BLOCK_SIZE);
    close(shm_fd);
}

void setup_DIR_block() {
    // mmap the shared memory object
    int shm_fd = shm_open(disk_name, O_RDWR, S_IRUSR | S_IWUSR);
    DISK_BLOCK *block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 * PAGE_SIZE);

    // use pointer arithmetic to get to the fourth block
    block = (DISK_BLOCK *) ((char *) block + 3 * BLOCK_SIZE);

    // Initialize the block after the FAT blocks as a directory block (root directory)
    block->dir.isdir = TRUE;
    block->dir.nextEntry = 0;
    /*
    block->dir.nextEntry = 1; // Set nextEntry to 1, since the first entry will be the root directory
    block->dir.dir_entry_list[0].isdir = 1; // Set the first entry as a directory
    block->dir.dir_entry_list[0].filefirstblock = 3; // Set the first block of the root directory (points to itself)
    strncpy(block->dir.dir_entry_list[0].name, "/", MAXNAME); // Set the name of the root directory entry
    */
    munmap(block, BLOCK_SIZE);
    close(shm_fd);

}



int pdos_mkfs(char *ID) {
    setup_ID_block(ID);
    setup_FAT_blocks();
    setup_DIR_block();
    return 0;
}