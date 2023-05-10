#include "pdosfilesys.h"

int pdos_close(PDOS_FILE *file) {
    // if the file is in write or read/write mode, write the buffer to the disk
    if (strcmp(file->mode, "w") == 0 || strcmp(file->mode, "rw") == 0) {
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

        // get the directory entry using the stored index
        DIR_ENTRY *dir_entry = &dir_block->dir.dir_entry_list[file->entrylistIdx];

        // write the buffer to the disk
        DISK_BLOCK *data_block = (DISK_BLOCK *) ((char *) admin_block + (file->blocknum * BLOCK_SIZE));
        memcpy(data_block, file->buffer, BLOCK_SIZE);

        // update the file length and modification time
        dir_entry->filelength = file->pos;
        dir_entry->filemodtime = time(NULL);

        // unmap the admin block
        if (munmap(admin_block, BLOCK_SIZE) == -1) {
            perror("munmap");
            close(shm_fd);
            exit(1);
        }

        // close the shared memory file descriptor
        if (close(shm_fd) == -1) {
            perror("close");
            exit(1);
        }
    }

    // free the file
    free(file->buffer);
    free(file);

    return 0;
}
