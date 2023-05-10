#include <sys/stat.h>
#include "../libPseudoFS/pdosfilesys.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <disk_name>\n", argv[0]);
        return 1;
    }
    pdos_mkdisk(argv[1]);
    return 0;
}