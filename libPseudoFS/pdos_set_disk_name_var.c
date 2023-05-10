#include "pdosfilesys.h"

char* disk_name = NULL;

/* 
 * note: because extern variables need to be defined with every execution of
 * a program, this function is a slight misnomer. It does not actually rename
 * the disk unless it is called in the user program pdos_mkdisk.c. Otherwise,
 * it just sets the value of the extern variable disk_name and validates that
 * the actual disk exists.
*/
int pdos_set_disk_name_var(char* name) {
    disk_name = name;
    return 0;
}


