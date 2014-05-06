/* File System Configuration*/

#define _MAX_FILES 3   /* Number of maximum files supported */
#define SECTOR_SIZE 16 /* Bytes */
#define FS_SIZE 8192	/* 128KB/16B= 8192 */
#define MAX_FILE_SIZE FS_SIZE/(_MAX_FILES+1)

typedef unsigned long uint32_t;

/* Define start physical address,           *
 * START_ADDRESS is from 9 flash sector     *
 * START_ADDRESS2 is from 10 flash sector   */
#define PHYSYCAL_START_ADDRESS2 ((uint32_t)0x080A0000)	// Sector  9
#define PHYSYCAL_START_ADDRESS ((uint32_t)0x080C0000)	// Sector 10
