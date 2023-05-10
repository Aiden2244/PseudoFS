#include "pdosfilesys.h"

PDOS_FILE *pdos_open(const char* fname, const char* mode) {

    // validate that the name is less than the max name
    if (strlen(fname) > MAXNAME) {
        printf("Error: File name is too long.\n");
        return NULL;
    }

    // validate that the mode is either "r", "w", or "rw"
    if (strcmp(mode, "r") != 0 && strcmp(mode, "w") != 0 && strcmp(mode, "rw") != 0) {
        printf("Error: Invalid mode.\n");
        return NULL;
    }

    // read the directory block into memory
    int shm_fd = shm_open(disk_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // we need to read the entire admin block into memory since mmap must be page aligned
    DISK_BLOCK *admin_block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (admin_block == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(1);
    }

    // use pointer arithmetic to get to the fourth block (directory block)
    DISK_BLOCK *dir_block = (DISK_BLOCK *) ((char *) admin_block + (3 * BLOCK_SIZE));

    // check the directory entries of the directory block for the file
    int i;
    for (i = 0; i < MAX_NUM_DIRECTORIES_ENTRIES; i++) {
        if (strcmp(dir_block->dir.dir_entry_list[i].name, fname) == 0) {
            break;
        }
    }

    // create the file to be returned
    PDOS_FILE *file = malloc(sizeof(PDOS_FILE));
    strcpy(file->mode, mode);
    file->pos = 0;
    file->buffer = NULL;

    // if i == MAX_NUM_DIRECTORIES_ENTRIES, then the file was not found
    if (i == MAX_NUM_DIRECTORIES_ENTRIES) {
        printf("File not found\n");

        // get the next available entry
        int next_entry = dir_block->dir.nextEntry;

        // if the next entry is the max number of entries, then the directory is full
        if (next_entry == MAX_NUM_DIRECTORIES_ENTRIES) {
            printf("Directory is full\n");
            free(file);
            return NULL;
        }

        // find the first free block in the FAT
        int free_block = -1;
        DISK_BLOCK *fat_block = (DISK_BLOCK *) ((char *) admin_block + (1 * BLOCK_SIZE));
        for (int j = 0; j < MAXBLOCKS; j++) {
            if (fat_block->fat[j] == UNUSED) {
                free_block = j;
                break;
            }
        }

        // if no free blocks, the disk is full
        if (free_block == -1) {
            printf("Disk is full\n");
            free(file);
            return NULL;
        }

        // mark the block as in use (end of a chain of 1)
        fat_block->fat[free_block] = ENDOFCHAIN;

        // set up the directory entry
        strncpy(dir_block->dir.dir_entry_list[next_entry].name, fname, MAXNAME);
        dir_block->dir.dir_entry_list[next_entry].filefirstblock = free_block;
        dir_block->dir.dir_entry_list[next_entry].filelength = 0;
        dir_block->dir.dir_entry_list[next_entry].isdir = 0;
        dir_block->dir.dir_entry_list[next_entry].filemodtime = time(NULL);

        // increment the next entry counter of the directory block
        dir_block->dir.nextEntry++;

        // set up the file descriptor
        
        file->blocknum = free_block;
        file->entrylistIdx = next_entry;
    }

    else {
        printf("File found\n");
        // check if the file is a directory
        if (dir_block->dir.dir_entry_list[i].isdir == 1) {
            printf("Error: File is a directory\n");
            free(file);
            return NULL;
        }

        // set up the file descriptor
        file->pos = dir_block->dir.dir_entry_list[i].filelength;
        file->blocknum = dir_block->dir.dir_entry_list[i].filefirstblock;
        file->entrylistIdx = i;

        // set up the buffer for the file descriptor


    }

    // close the shared memory
    if (munmap(admin_block, BLOCK_SIZE) == -1) {
        perror("munmap");
        close(shm_fd);
        exit(1);
    }

    if (close(shm_fd) == -1) {
        perror("close");
        exit(1);
    }

    return file;

}