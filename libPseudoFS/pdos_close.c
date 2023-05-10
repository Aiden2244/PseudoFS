#include "pdosfilesys.h"

int pdos_close(PDOS_FILE *file) {
    // if the file is in write or read/write mode, write the buffer to the disk
    if (strcmp(file->mode, "w") == 0 || strcmp(file->mode, "rw") == 0) {
        pdos_flush(file);
    }

    // free the file
    free(file->buffer);
    free(file);

    return 0;
}
