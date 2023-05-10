/*
 * Definitions and Description for File Allocation Table
 * structure
 *
 */

 #ifndef PDOSFILESYS_H
 #define PDOSFILESYS_H

/* includes needed for most used headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

/* 
 * INFO about Disk Blocks
 */

#define BLOCK_SIZE	1024
#define MAXBLOCKS	1024
// Block status
#define UNUSED		-1
#define ENDOFCHAIN	0
// directory entries
#define MAXNAME		13 // old DOS format  filename.txt = 13 bytes (includeing '\0')
#define MAXPATHLENGTH	512

#define TRUE	1

#define PAGE_SIZE 	4096 
/*
 * A structure for directory entries. Each file/dir in a directory block 
 * is represented by one of these
 */

typedef struct direntry {
	char 	isdir;			// ture if this entry is a directory entry (used for sub directory)
	time_t	filemodtime;	// last time file was access
	
    int     filelength ;	// length of file in bytes
	short	filefirstblock;	// FAT index for this file
	char	name[MAXNAME];	// file name
} DIR_ENTRY;

#define MAX_NUM_DIRECTORIES_ENTRIES ((BLOCK_SIZE - (2 * sizeof(int)) ) / sizeof(DIR_ENTRY))
/*
 * Directoy Block, occupies a whole disk block
 * Every file system must have at lease one of these, which is the
 * root directory.
 * An empty root directory will have this with its nextentry = 0;
 */
typedef struct DIRBLOCK{
	int isdir;	// identifies this directory as a directory block
	int nextEntry;	// points to the next available entry (if == 0, means empty directory)
	DIR_ENTRY dir_entry_list[MAX_NUM_DIRECTORIES_ENTRIES];
} DIRECTORY_BLOCK;


/*
 * FAT BLOCK
 * Array of short ints
 * one for each disk block, of which there is 1024
 * Therefore 
 *		assume FAT occupies one block (1024 bytes)
 		we can have a maximum of 512 entires. 512 * 2 bytes = 1024 bytes
 		so, a FAT[512] takes up one block. 
 		We need two of this if we want to account for all 1024 disk blocks
 */

typedef short FAT_BLOCK[ BLOCK_SIZE / 2];


/* Last type of disk block is a data block. Holds user data
*/

typedef char DATA_BLOCK[BLOCK_SIZE];

/* 
 * Three different types of blocks
 * So use a union
 *		it allows you to allocate or declare a uniform block 
 *		and then use it based on the type of block it is used to represent 
 *		NOTE: union creates storage large enough for the largest
 *		data type..
 */
typedef union block {
	DIRECTORY_BLOCK	dir;
	FAT_BLOCK 		fat;
	DATA_BLOCK 		data;

} DISK_BLOCK;

/* 
 * File Descriptor --
 *		similar to file descriptor in LINUX
 *		used to store information about current opened
 *		file
 */
typedef struct FILE_DESCRIPTOR{
	char 		mode[3];	// "r", "w" or "rw"
	int 		pos;		// position in current block (byte location of next read/write in block)
	short		blocknum;	// Number of the block being used
    DISK_BLOCK      buffer;	// buffered reads and writes. Flush buffer when you access another block or fishish r/w
    unsigned char entrylistIdx;  // index in directory block

} PDOS_FILE;

/*
 * The name of the file system
 */
extern char* disk_name;

/*
 * funtion declarations
 */
int pdos_set_disk_name_var(char *name);
int pdos_mkfs(char *ID);
int pdos_mkdisk(int sz);
PDOS_FILE * pdos_open(const char *fname, const char *mode);

#endif /* PDOSFILESYS_H */