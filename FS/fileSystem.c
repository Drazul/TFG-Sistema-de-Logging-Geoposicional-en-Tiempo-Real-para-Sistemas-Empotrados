#include "fileSystem.h"

static FS *fs[2];
static int actual_fs = 0;

uint32_t byte_2_uint32(BYTE* src) {

	uint32_t dst = 0;
	/*
	 dst |= (uint32_t) src[0] << 0;
	 dst |= (uint32_t) src[1] << 8;
	 dst |= (uint32_t) src[2] << 16;
	 dst |= (uint32_t) src[3] << 24;
	 */
	dst = (uint32_t) src[0] << 24;
	dst |= (uint32_t) src[1] << 26;
	dst |= (uint32_t) src[2] << 8;
	dst |= (uint32_t) src[3] << 0;
	return dst;
}

BYTE* uint32_2_byte(uint32_t src, BYTE * dst) {
	dst[3] = (BYTE) (src >> 24);
	dst[2] = (BYTE) (src >> 16);
	dst[1] = (BYTE) (src >> 8);
	dst[0] = (BYTE) (src >> 0);

	return dst;
}

FRESULT check_fs(int fsIndex) {

	FRESULT result = FR_OK;
	BYTE check[11] = { 'F', 'S', ' ', 'C', 'O', 'R', 'R', 'E', 'C', 'T', 'O' };
	int i;
	result = disk_initialize(fsIndex);
	if (result != FR_OK)
		return FR_NOT_READY;

	result = disk_read(fsIndex, fs[fsIndex]->win, 0, 1); /* Read sector 0 */
	if (result != FR_OK)
		return FR_DISK_ERR;

	for (i = 0; i < 11; i++)
		if (fs[fsIndex]->win[i] != check[i])
			return FR_NO_FILESYSTEM;

	return FR_OK;
}

FRESULT check_file(FIL *file) {

	int i;
	/* Each file has 1 Byte validity, 4 bytes for start address, 4 bytes for end address, 7 bytes for file name */

	if (fs[file->fsIndex]->win[0] != 255) /*Invalid entry, read next*/
		return FR_INVALID_OBJECT; /* 255 if when a byte is empty*/

	file->dirty = 255;

	file->startSector = fs[file->fsIndex]->win[1] << 24;
	file->startSector |= fs[file->fsIndex]->win[2] << 16;
	file->startSector |= fs[file->fsIndex]->win[3] << 8;
	file->startSector |= fs[file->fsIndex]->win[4] << 0;

	file->writePointer = fs[file->fsIndex]->win[5] << 24;
	file->writePointer |= fs[file->fsIndex]->win[6] << 16;
	file->writePointer |= fs[file->fsIndex]->win[7] << 8;
	file->writePointer |= fs[file->fsIndex]->win[8] << 0;

	file->readPointer = 0;
	file->flag = 0;
	file->err = 0;
	file->buffindex = 0;

	if ((file->startSector == 4294967295)
			&& (file->writePointer == 4294967295)) { /* This number is if buff is empty*/
		file->err = 1;
		return FR_NO_FILE; /* No more fat entries, no more files */
	}

	for (i = 0; i < 7; i++)
		file->name[i] = fs[file->fsIndex]->win[i + 9];

	return FR_OK;
}

FRESULT read_file_entry(FIL *file, DWORD sector) {

	FRESULT result = FR_OK;
	result = disk_read(file->fsIndex, fs[file->fsIndex]->win, sector, 1);
	if (result != FR_OK)
		return FR_DISK_ERR;

	result = check_file(file);
	if (result == FR_OK)
		file->descriptorSector = sector;
	return result;
}

FRESULT loading_files(int fsIndex) {

	FRESULT result = FR_OK;
	DWORD sector;

	int nfiles = 0;
	for (sector = fs[fsIndex]->fatbase; sector < fs[fsIndex]->database;
			sector++) {
		fs[fsIndex]->files[nfiles].fsIndex = fsIndex;
		result = read_file_entry(&fs[fsIndex]->files[nfiles], sector);
		if (result == FR_OK)
			nfiles++;

		if (result == FR_NO_FILE) {
			fs[fsIndex]->lastDescriptor = sector;
			break; /* if no more files then is the last sector */
		}
		if (nfiles == _MAX_FILES) {
			fs[fsIndex]->lastDescriptor = sector + 1;
			break; /* if no more files then is the last sector */
		}
	}
	if (nfiles < _MAX_FILES)
		for (; nfiles < _MAX_FILES; nfiles++) {
			fs[fsIndex]->files[nfiles].err = 1;
		}
	return FR_OK;
}

int compare(const TCHAR *path, TCHAR *name) {
	int i;
	for (i = 0; i < 7; i++) { /* 7 is the max length of name */
		if (path[i] != name[i])
			return 0;
	}

	return 1;
}

int update_file(FIL *src, FIL *dst) {
	dst->buffindex = src->buffindex;
	dst->descriptorSector = src->descriptorSector;
	dst->dirty = src->dirty;
	dst->err = src->err;
	dst->flag = src->flag;
	dst->readPointer = src->readPointer;
	dst->startSector = src->startSector;
	dst->writePointer = src->writePointer;
	dst->fsIndex = src->fsIndex;
	int i;
	for (i = 0; i < 7; i++)
		dst->name[i] = src->name[i];

	return 1;
}

FRESULT copy_file(FIL *src, FIL *dst) {
	FRESULT result = FR_OK;
	UINT byteRead = 0, byteWrite = 0;
	f_open(dst, (TCHAR*) src->name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	while (src->readPointer < src->writePointer) { /* ReadPointer se actualiza al leer */
		f_read(src, fs[src->fsIndex]->win, SECTOR_SIZE, &byteRead);
		f_write(dst, fs[src->fsIndex]->win, byteRead, &byteWrite);
	}

	f_close(src);
	f_close(dst);

	return result;
}

FRESULT backup_fs(int src, int dst) {
	FRESULT result = FR_OK;
	int i;
	for (i = 0; i < _MAX_FILES; i++) {
		if (fs[src]->files[i].err == -1)
			break;
		result = copy_file(&fs[src]->files[i], &fs[dst]->files[i]);
		if (result != FR_OK)
			return result;
	}

	return result;
}

FRESULT close_all_files() {
	FRESULT result = FR_OK;
	int i;
	for (i = 0; i < _MAX_FILES; i++)
		if (fs[actual_fs]->files[i].err == 0)
			f_close(&fs[actual_fs]->files[i]);
	return result;
}

FRESULT reset_sector(int fsIndex) {
	FRESULT result = FR_OK;
	uint32_t address;
	if (fsIndex == 0)
		address = PHYSYCAL_START_ADDRESS;
	else
		address = PHYSYCAL_START_ADDRESS2;

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	if (FLASH_EraseSector(GetSector(address), VoltageRange_3) != FLASH_COMPLETE)
		result = FR_DISK_ERR;

	return result;
}

FRESULT change_sector() {
	FRESULT result = FR_OK;
	FS fs1, fs2;
	if (actual_fs == 0) {
		f_mount(&fs1, 1, 0);
		f_mkfs(1);
		f_mount(&fs1, 1, 1);
		f_mount(&fs2, 0, 1);
		actual_fs = 1;
		result = backup_fs(0, 1);
		reset_sector(0);
	} else {
		f_mount(&fs1, 0, 0);
		f_mkfs(0);
		f_mount(&fs1, 0, 1);
		f_mount(&fs2, 1, 1);
		actual_fs = 0;
		result = backup_fs(1, 0);
		reset_sector(1);
	}
	return result;
}

/* Open or create a file */
FRESULT f_open(FIL* fp, /* Pointer to the blank file object */
const TCHAR* path, /* Pointer to the file name */
BYTE mode /* Access mode and file open mode flags */
) {
	FRESULT result = FR_OK;
	int i, j;
	int find = 0;

	if (!fp)
		return FR_INVALID_OBJECT;
	mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS
			| FA_CREATE_NEW;

	for (i = 0; i < _MAX_FILES && find == 0; i++) {
		if (fs[actual_fs]->files[i].err == 1)
			break;
		if (compare(path, (TCHAR*) fs[actual_fs]->files[i].name) == 1) {
			fs[actual_fs]->files[i].flag = mode;
			find = 1;
			break;
		}
	}
	if (i == _MAX_FILES && find == 0)
		return FR_TOO_MANY_FILES;
	if (find == 0
			&& (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW))) {

		for (j = 0; j < 7; j++) {
			fs[actual_fs]->files[i].name[j] = (BYTE) path[j];
		}
		fs[actual_fs]->files[i].dirty = 1; /* Created is dirty */
		fs[actual_fs]->files[i].flag = mode;
		fs[actual_fs]->files[i].fsIndex = actual_fs;
		fs[actual_fs]->files[i].startSector = fs[actual_fs]->database * (i + 1);
		fs[actual_fs]->files[i].readPointer = 0;
		fs[actual_fs]->files[i].writePointer = 0;
		fs[actual_fs]->files[i].err = 0;
		fs[actual_fs]->files[i].buffindex = 0;
		fs[actual_fs]->files[i].descriptorSector = 0;

	} else if (find == 0)
		result = FR_NO_FILE;

	update_file(&fs[actual_fs]->files[i], fp);
	return result;
}

/* Close an open file object */
FRESULT f_close(FIL *fp /* Pointer to the file object to be closed */) {
	FRESULT res = FR_OK;
	if (fp->buffindex != 0) {
		f_sync(fp);
	}

	if (fp->dirty != 255) {
		if (fp->descriptorSector != 0) { /* Si es 0 quiere decir que no tiene ningun descriptor */
			fs[fp->fsIndex]->win[0] = 0; /* Para ponerlo como invalido*/
			disk_write(fp->fsIndex, fs[fp->fsIndex]->win, fp->descriptorSector,
					1);
		}

		fp->buff[0] = 255;

		fp->buff[1] = (BYTE) (fp->startSector >> 24);
		fp->buff[2] = (BYTE) (fp->startSector >> 16);
		fp->buff[3] = (BYTE) (fp->startSector >> 8);
		fp->buff[4] = (BYTE) (fp->startSector >> 0);

		fp->buff[5] = (BYTE) (fp->writePointer >> 24);
		fp->buff[6] = (BYTE) (fp->writePointer >> 16);
		fp->buff[7] = (BYTE) (fp->writePointer >> 8);
		fp->buff[8] = (BYTE) (fp->writePointer >> 0);

		fp->buff[9] = fp->name[0];
		fp->buff[10] = fp->name[1];
		fp->buff[11] = fp->name[2];
		fp->buff[12] = fp->name[3];
		fp->buff[13] = fp->name[4];
		fp->buff[14] = fp->name[5];
		fp->buff[15] = fp->name[6];

		fp->descriptorSector = fs[fp->fsIndex]->lastDescriptor;

		disk_write(fp->fsIndex, fp->buff, fs[fp->fsIndex]->lastDescriptor++, 1); // lastDescriptor is the number of sector

		fp->dirty = 255;
	}

	int i;
	for (i = 0; i < _MAX_FILES; i++) {
		if (compare((TCHAR*) fp->name, (TCHAR*) fs[fp->fsIndex]->files[i].name)
				== 1)
			break;
	}
	update_file(fp, &fs[actual_fs]->files[i]);

	return res;
}

/* Read data from a file */
FRESULT f_read(FIL* fp, /* Pointer to the file object */
void* buff, /* Pointer to data buffer */
UINT btr, /* Number of bytes to read */
UINT* br /* Pointer to number of bytes read */
) {
	FRESULT result = FR_OK;
	BYTE *rbuff = (BYTE*) buff;
	if (((fp->writePointer + fp->buffindex) - fp->readPointer) < btr)
		btr = (fp->writePointer + fp->buffindex) - fp->readPointer;
	int sector = fp->startSector + (fp->readPointer / SECTOR_SIZE);
	int offset = fp->readPointer % SECTOR_SIZE;
	UINT counter = 0;

	while (counter < btr) {
		disk_read(fp->fsIndex, fs[fp->fsIndex]->win, sector++, 1);
		while (offset < SECTOR_SIZE) {
			rbuff[counter++] = fs[fp->fsIndex]->win[offset++];
			if (counter == btr) {
				fp->readPointer += counter;
				*br = counter;
				break;
			}
		}
		offset = 0;
	}

	return result;
}

/* Write data to a file */
FRESULT f_write(FIL* fp, /* Pointer to the file object */
const void *buff, /* Pointer to the data to be written */
UINT btw, /* Number of bytes to write */
UINT* bw /* Pointer to number of bytes written */
) {
	FRESULT result = FR_OK;
	BYTE *rbuff = (BYTE*) buff;
	fp->dirty = 1;
	DWORD sector = fp->startSector + (fp->writePointer / SECTOR_SIZE);
	int index = 0, offset;
	offset = fp->writePointer % SECTOR_SIZE;
	/* Quiere decir que el ultimo sector escrito no esta completo.
	 * Si buffindex no es 0 esto ya se ha comprobado antes.
	 * Puede haber offset!=0 y buffindex!=0 cuando aun no hay suficiente para escribir un sector */
	if (offset != 0 && fp->buffindex == 0) {
		disk_read(fp->fsIndex, fs[fp->fsIndex]->win, sector, 1);
		for (; fp->buffindex < SECTOR_SIZE; fp->buffindex++) {
			if (fs[fp->fsIndex]->win[fp->buffindex] == 255)
				break;
			fp->buff[fp->buffindex] = fs[fp->fsIndex]->win[fp->buffindex];
		}
		fp->writePointer -= offset;
	}

	while (index < btw) {
		fp->buff[fp->buffindex++] = rbuff[index++];
		if (fp->buffindex == SECTOR_SIZE) {
			fp->buffindex = 0;
			disk_write(fp->fsIndex, fp->buff, sector++, 1);
			fp->writePointer += 16;
		}
	}

	*bw = btw;
	return result;
}

/* Move file pointer of a file object */
FRESULT f_truncateStart(FIL* fp, /* Pointer to the file object */
DWORD ofs /* File pointer from top of file */
) {
	FRESULT result = FR_OK;

	fp->startSector += ofs;
	fp->writePointer -= ofs;
	if (fp->writePointer < 0)
		fp->writePointer = 0;
	fp->readPointer -= ofs;
	if (fp->readPointer < 0)
		fp->readPointer = 0;

	return result;
}

/* Flush cached data of a writing file */
FRESULT f_sync(FIL* fp /* Pointer to the file object */
) {
	DRESULT result;
	DWORD sector = fp->startSector + (fp->writePointer / SECTOR_SIZE);
	fp->writePointer += fp->buffindex;
	for (; fp->buffindex < SECTOR_SIZE; fp->buffindex++)
		fp->buff[fp->buffindex] = 255;

	fp->buffindex = 0;
	result = disk_write(fp->fsIndex, fp->buff, sector, 1);
	return result;
}

/* Get min free space for a file */
FRESULT f_getfree(int fsIndex, /*Index of fileSystem */
UINT* mnfs /* Pointer to a variable to return number of free sectors */
) {
	FRESULT result = FR_OK;
	int i, minFreeSectors = 0, freeSectors = 0;

	minFreeSectors = MAX_FILE_SIZE - fs[fsIndex]->lastDescriptor;

	for (i = 0; i < _MAX_FILES; i++) {
		if (fs[fsIndex]->files[i].err != -1) {
			/* i+2 porque se contempla el tamaño de un archivo extra para descriptores del FS */
			freeSectors =
					(MAX_FILE_SIZE * (i + 2))
							- ((fs[fsIndex]->files[i].startSector
									+ fs[fsIndex]->files[i].writePointer)
									/ SECTOR_SIZE);
			if (freeSectors < minFreeSectors)
				minFreeSectors = freeSectors;
		}
	}
	mnfs = (UINT*) minFreeSectors;
	return result;
}

/* Mount/Unmount a logical drive */
FRESULT f_mount(FS * fileSystem, int fsIndex, /*Index of fileSystem */
BYTE opt /* 0:Do not mount (delayed mount), 1:Mount immediately */
) {
	FRESULT result = FR_OK;
	fs[fsIndex] = fileSystem;
	if (fsIndex == 0)
		fileSystem->start_address = PHYSYCAL_START_ADDRESS;
	else
		fileSystem->start_address = PHYSYCAL_START_ADDRESS2;

	fileSystem->sector_size = SECTOR_SIZE;
	fileSystem->fs_size = FS_SIZE;
	fileSystem->free_sector = FS_SIZE;
	fileSystem->volbase = 0;
	fileSystem->fatbase = 2; /* Because only need 1 sector for FS info */
	fileSystem->database = FS_SIZE / (_MAX_FILES + 1); /* Same size for fs info than file data*/

	if (opt != 1)
		return FR_OK;

	result = check_fs(fsIndex);
	if (result != FR_OK)
		return FR_NO_FILESYSTEM;

	result = loading_files(fsIndex);
	if (result != FR_OK)
		return FR_NO_FILE;

	return FR_OK;
}

/* Create a file system on the volume */
FRESULT f_mkfs(int fsIndex/*Index of fileSystem */) {

	FRESULT result = FR_OK;
	BYTE buff[SECTOR_SIZE] = { 'F', 'S', ' ', 'C', 'O', 'R', 'R', 'E', 'C', 'T',
			'O' };

	fs[fsIndex]->win[0] = buff[0];
	fs[fsIndex]->win[1] = buff[1];
	fs[fsIndex]->win[2] = buff[2];
	fs[fsIndex]->win[3] = buff[3];
	fs[fsIndex]->win[4] = buff[4];
	fs[fsIndex]->win[5] = buff[5];
	fs[fsIndex]->win[6] = buff[6];
	fs[fsIndex]->win[7] = buff[7];
	fs[fsIndex]->win[8] = buff[8];
	fs[fsIndex]->win[9] = buff[9];
	fs[fsIndex]->win[10] = buff[10];
	fs[fsIndex]->win[11] = buff[11];
	fs[fsIndex]->win[12] = buff[12];
	fs[fsIndex]->win[13] = buff[13];
	fs[fsIndex]->win[14] = buff[14];
	fs[fsIndex]->win[15] = buff[15];
	fs[fsIndex]->win[16] = buff[16];

	result = disk_initialize(fsIndex);
	if (result != FR_OK)
		return FR_NOT_READY;

	result = disk_write(fsIndex, fs[fsIndex]->win, 0, 1);
	if (result != FR_OK)
		return FR_DISK_ERR;

	return FR_OK;
}
