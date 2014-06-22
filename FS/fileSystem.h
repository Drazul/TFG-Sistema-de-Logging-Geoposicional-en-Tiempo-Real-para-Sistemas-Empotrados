#include "integer.h"
#include "stm32f2xx.h"
#include "diskio.h"

/* FILE PERMISSIONS */
#define	 FA_READ			0x01
#define	 FA_OPEN_EXISTING	0x00

#define FA_WRITE			0x02
#define	 FA_CREATE_NEW		0x04
#define	 FA_CREATE_ALWAYS	0x08
#define	 FA_OPEN_ALWAYS		0x10

/* STATUS CODES */
typedef enum {
	FR_OK = 0, /* (0) Succeeded */
	FR_DISK_ERR, /* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR, /* (2) Assertion failed */
	FR_NOT_READY, /* (3) The physical drive cannot work */
	FR_NO_FILE, /* (4) Could not find the file */
	FR_NO_PATH, /* (5) Could not find the path */
	FR_INVALID_NAME, /* (6) The path name format is invalid */
	FR_DENIED, /* (7) Access denied due to prohibited access or directory full */
	FR_EXIST, /* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT, /* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED, /* (10) The physical drive is write protected */
	FR_INVALID_DRIVE, /* (11) The logical drive number is invalid */
	FR_NOT_ENABLED, /* (12) The volume has no work area */
	FR_NO_FILESYSTEM, /* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED, /* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT, /* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED, /* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE, /* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_FILES, /* (18) Number of  files > _MAX_FILES */
	FR_INVALID_PARAMETER /* (19) Given parameter is invalid */
} FRESULT;

/* File structure*/
typedef struct {
	BYTE name[7];
	BYTE flag; /* File status flags */
	BYTE err; /* Abort flag (error code) */
	BYTE dirty; /* If 1 then need write in disk*/
	BYTE fsIndex; /* Index of file system */
	DWORD startSector; /* File Start Sector */
	DWORD writePointer; /* File End Sector. Current write pointer, in bytes */
	DWORD readPointer; /* File Read Pointer. in bytes */
	DWORD descriptorSector; /* Sector of FS descriptor for file */
	WORD buffindex; /* Index of last buff write position */
	BYTE buff[SECTOR_SIZE];/* File data read/write buffer */
} FIL;

/* File System Structure */
typedef struct {
	uint32_t start_address; /* Start Address of the file system */
	BYTE sector_size; /* Size of each sector */
	WORD free_sector; /* Number of free sectors */
	WORD fs_size; /* Sectors per FS */
	WORD volbase; /* Volume start sector */
	WORD fatbase; /* FS start sector */
	WORD database; /* Data start sector */
	WORD lastDescriptor; /* Sector last file descriptor */
	FIL files[_MAX_FILES]; /* Total of open files */
	BYTE win[SECTOR_SIZE]; /* FS read/write buffer */
} FS;

/* FUNTIONS SUPORTED */
FRESULT f_open(FIL* fp, const TCHAR* path, BYTE mode); /* Open or create a file */
FRESULT f_close(FIL* fp); /* Close an open file object */
FRESULT f_read(FIL* fp, void* buff, UINT btr, UINT* br); /* Read data from a file */
FRESULT f_write(FIL* fp, const void* buff, UINT btw, UINT* bw); /* Write data to a file */
FRESULT f_truncateStart(FIL* fp, DWORD ofs); /* Move start file pointer of a file object */
FRESULT f_lseek(FIL* fp, DWORD ofs); /* Move read pointer of a file object  */
FRESULT f_sync(FIL* fp); /* Flush cached data of a writing file */
FRESULT f_getfree(int fsIndex, UINT* nclst); /* Get number of free clusters on the drive */
FRESULT f_mount(FS* fileSystem, int fsIndex, BYTE opt); /* Mount/Unmount a logical drive */
FRESULT f_mkfs(int fsIndex); /* Create a file system on the volume */
FRESULT reset_sector(int fsIndex);
