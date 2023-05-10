#include <sys/stat.h>
#include "../libPseudoFS/pdosfilesys.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <disk_name>\n", argv[0]);
        return 1;
    }
    int sz = BLOCK_SIZE * MAXBLOCKS;
    
    // here is the only time this call actually sets the name of the disk being made
    pdos_set_disk_name_var(argv[1]);

    pdos_mkdisk(sz);
    return 0;
}