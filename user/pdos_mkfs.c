#include <sys/stat.h>
#include "../libPseudoFS/pdosfilesys.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <disk_name> <fs_name>\n", argv[0]);
        return 1;
    }

    // note: this is not renaming the file system but checking if the passed argument
    // corresponds to an existing file system
    pdos_set_disk_name_var(argv[1]);

    pdos_mkfs(argv[2]);
    return 0;
}