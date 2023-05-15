/* 
 * File:   pseudoWriteFS.c
 * Author: osboxes
 * 1. Create a new file on pseudoFS
 * 2. Write out a single block (1024) characters (A-Z)
 * 3. Close file
 * *
 * * Check file system using hexdump -C MYFS
 *
 * Created on April 2023, 1:29 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "../libPseudoFS/pdosfilesys.h"
/*
 * 
 */
int main(int argc, char** argv) {
  
   
   PDOS_FILE *fd = NULL;
   pdos_set_disk_name_var("foo");
    
    

   // open file for read/writing
   fd = pdos_open("file1.txt", "rw");
   
   
   int charValue = 'A';  // Letter A

   for(int i = 0; i < BLOCK_SIZE -1; i++, charValue++){
   if (charValue > 'Z') {
         charValue = 'A';
      }
      pdos_fputc(charValue, fd);
   }


   char first = pdos_fgetc(fd);
   printf("First character in file: %c\n", first);

   
   // close file
   pdos_close(fd);



   //return (EXIT_SUCCESS);

   return 0;
}