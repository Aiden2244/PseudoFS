#include "pdosfilesys.h"

PDOS_FILE *pdos_open(const char* fname, const char* mode) {

    printf("Opening file %s in mode %s\n", fname, mode);
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
    printf("i: %d\n", i);
    printf("max entries: %d\n", (int) MAX_NUM_DIRECTORIES_ENTRIES);

    // create the file to be returned
    PDOS_FILE *file = malloc(sizeof(PDOS_FILE));
    strcpy(file->mode, mode);
    file->pos = 0;
    file->buffer = (DISK_BLOCK *) malloc(BLOCK_SIZE);

    // if i == MAX_NUM_DIRECTORIES_ENTRIES, then the file was not found
    if (i == MAX_NUM_DIRECTORIES_ENTRIES) {
        printf("The file %s did not exist. Creating it now...\n", fname);
        // get the next available entry
        int next_entry = dir_block->dir.nextEntry;
        printf("next entry: %d\n", next_entry);
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

        printf("first free block: %d\n", free_block);

        // mark the block as in use (end of a chain of 1)
        fat_block->fat[free_block] = ENDOFCHAIN;

        // set up the directory entry
        strncpy(dir_block->dir.dir_entry_list[next_entry].name, fname, MAXNAME);
        printf("name: %s\n", dir_block->dir.dir_entry_list[next_entry].name);
        dir_block->dir.dir_entry_list[next_entry].filefirstblock = free_block;
        printf("first block: %d\n", dir_block->dir.dir_entry_list[next_entry].filefirstblock);
        dir_block->dir.dir_entry_list[next_entry].filelength = 0;
        printf("length: %d\n", dir_block->dir.dir_entry_list[next_entry].filelength);
        dir_block->dir.dir_entry_list[next_entry].isdir = 0;
        printf("isdir: %d\n", dir_block->dir.dir_entry_list[next_entry].isdir);
        dir_block->dir.dir_entry_list[next_entry].filemodtime = time(NULL);
        printf("modtime: %ld\n", dir_block->dir.dir_entry_list[next_entry].filemodtime);

        // increment the next entry counter of the directory block
        dir_block->dir.nextEntry++;

        // set up the file descriptor
        
        file->blocknum = free_block;
        printf("blocknum: %d\n", file->blocknum);
        file->entrylistIdx = next_entry;
        printf("entrylistIdx: %d\n", file->entrylistIdx);
    }

    else {
        printf("The file %s was found.\n", fname);
        // check if the file is a directory
        if (dir_block->dir.dir_entry_list[i].isdir == 1) {
            printf("Error: File is a directory\n");
            free(file);
            return NULL;
        }

        // set up the file descriptor
        printf("pos: %d\n", file->pos);
        file->blocknum = dir_block->dir.dir_entry_list[i].filefirstblock;
        printf("blocknum: %d\n", file->blocknum);
        file->entrylistIdx = i;
        printf("entrylistIdx: %d\n", file->entrylistIdx);

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