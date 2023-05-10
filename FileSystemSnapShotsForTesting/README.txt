The following directory contains 3 sub directories. Each subdirectory contains a snap shot of the file system after a particular operation. 
You can use these files (they are actual shared-memory object files for our in-memory file system) for developer testing. I hope it helps. 
 
1. subdirectory FileSystemSnapShotRawDisk
*  pdos_mkdisk. This is the raw shared memory created for our FS. No structures applied yet. We just created the disk.

2. sub directory FileSystemSnapShot_Empty_FAT_DIR 
*  Just the first 4 administrative blocks (1 Reserve Block, 2 FAT and one Directory Block)

3. sub directory FileSystemSnapShot_File1_1023
*  FS continaing one file (file1.txt) with 1023 (upper-case characters)


You can copy these files (only one can exist in /dev/shm at any given time) to /dev/shm and use them as if you created them. If you do, ensure they are owned by cosc255
and have rw-rw-r-- permission.

jm
