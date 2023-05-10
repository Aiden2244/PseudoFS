#include <sys/stat.h>
#include "../libPseudoFS/pdosfilesys.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <disk_name> <fs_name>\n", argv[0]);
        return 1;
    }
    pdos_mkfs(argv[1], argv[2]);
    return 0;
}