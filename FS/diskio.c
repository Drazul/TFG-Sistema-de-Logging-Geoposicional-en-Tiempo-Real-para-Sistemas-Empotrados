#include "diskio.h"		/* FileSystem lower layer API */
#include <stm32f2xx_flash.h>
#include <string.h>

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector  0,  16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector  1,  16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector  2,  16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector  3,  16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector  4,  64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector  5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector  6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector  7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector  8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector  9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

uint32_t GetSector(uint32_t Address) {
	uint32_t sector = 0;

	if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)) {
		sector = FLASH_Sector_0;
	} else if ((Address < ADDR_FLASH_SECTOR_2)
			&& (Address >= ADDR_FLASH_SECTOR_1)) {
		sector = FLASH_Sector_1;
	} else if ((Address < ADDR_FLASH_SECTOR_3)
			&& (Address >= ADDR_FLASH_SECTOR_2)) {
		sector = FLASH_Sector_2;
	} else if ((Address < ADDR_FLASH_SECTOR_4)
			&& (Address >= ADDR_FLASH_SECTOR_3)) {
		sector = FLASH_Sector_3;
	} else if ((Address < ADDR_FLASH_SECTOR_5)
			&& (Address >= ADDR_FLASH_SECTOR_4)) {
		sector = FLASH_Sector_4;
	} else if ((Address < ADDR_FLASH_SECTOR_6)
			&& (Address >= ADDR_FLASH_SECTOR_5)) {
		sector = FLASH_Sector_5;
	} else if ((Address < ADDR_FLASH_SECTOR_7)
			&& (Address >= ADDR_FLASH_SECTOR_6)) {
		sector = FLASH_Sector_6;
	} else if ((Address < ADDR_FLASH_SECTOR_8)
			&& (Address >= ADDR_FLASH_SECTOR_7)) {
		sector = FLASH_Sector_7;
	} else if ((Address < ADDR_FLASH_SECTOR_9)
			&& (Address >= ADDR_FLASH_SECTOR_8)) {
		sector = FLASH_Sector_8;
	} else if ((Address < ADDR_FLASH_SECTOR_10)
			&& (Address >= ADDR_FLASH_SECTOR_9)) {
		sector = FLASH_Sector_9;
	} else if ((Address < ADDR_FLASH_SECTOR_11)
			&& (Address >= ADDR_FLASH_SECTOR_10)) {
		sector = FLASH_Sector_10;
	} else //(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))
	{
		sector = FLASH_Sector_11;
	}

	return sector;
}

uint32_t byte_to_uint32(const BYTE* src) {
	uint32_t dst = 0;
	dst |= (uint32_t) src[0] << 0;
	dst |= (uint32_t) src[1] << 8;
	dst |= (uint32_t) src[2] << 16;
	dst |= (uint32_t) src[3] << 24;

	return dst;
}

BYTE* uint32_to_byte(uint32_t src, BYTE * dst) {
	dst[3] = (BYTE) (src >> 24);
	dst[2] = (BYTE) (src >> 16);
	dst[1] = (BYTE) (src >> 8);
	dst[0] = (BYTE) (src >> 0);

	return dst;
}

DSTATUS get_start_end_address(int pdrv, int count, int sector,
		uint32_t* startAddress, uint32_t* endAddress) {
	int size;
	disk_ioctl(pdrv, GET_SECTOR_SIZE, &size);

	if (pdrv == 0) {
		startAddress = (uint32_t*) PHYSYCAL_START_ADDRESS + (size * sector);
		endAddress = (uint32_t*) startAddress + (size * count);
	} else {
		startAddress = (uint32_t*) PHYSYCAL_START_ADDRESS2 + (size * sector);
		endAddress = (uint32_t*) startAddress + (size * count);
	}
	return RES_OK;
}

uint32_t get_address(int pdrv, DWORD sector) {
	uint32_t address;
	int size;
	disk_ioctl(pdrv, GET_SECTOR_SIZE, &size);
	if (pdrv == 0)
		address = (uint32_t) PHYSYCAL_START_ADDRESS + (size * sector);
	else
		address = (uint32_t) PHYSYCAL_START_ADDRESS2 + (size * sector);

	return address;
}

DSTATUS disk_initialize(int pdrv /* Physical drive number (0..) */
) {
	if (pdrv != 0 && pdrv != 1)
		return RES_PARERR;

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	FLASH_Lock();

	return RES_OK;

}

DSTATUS flashState2FSState(FLASH_Status status) {
	if (status == FLASH_ERROR_WRP)
		return STA_PROTECT;
	return 0; //Status OK
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(int pdrv /* Physical drive number (0..) */
) {
	if (pdrv != 0 && pdrv != 1)
		return RES_PARERR;
	FLASH_Status state = FLASH_GetStatus();

	return flashState2FSState(state);
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(int pdrv, /* Physical drive number (0..) */
BYTE *buff, /* Data buffer to store read data */
DWORD sector, /* Sector address (LBA) */
UINT count /* Number of sectors to read (1..128) */
) {

	if ((pdrv != 0 && pdrv != 1) || !count)
		return RES_PARERR;

	DSTATUS status;
	uint32_t endAddress = 0, address = 0;
	uint32_t read = 0;

	address = get_address(pdrv, sector);
	endAddress = get_address(pdrv, sector + count);

	status = disk_ioctl(pdrv, CTRL_SYNC, (void*) NULL);

	if (status != RES_OK)
		return status;

	do {
		read = *(__IO uint32_t*) address;
		uint32_to_byte(read, buff);

		buff += 4;
		address += 4;

		status = disk_ioctl(pdrv, CTRL_SYNC, (void*) NULL);
		if (status != RES_OK)
			return status;

	} while (address < endAddress);

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(int pdrv, /* Physical drive number (0..) */
const BYTE *buff, /* Data to be written */
DWORD sector, /* Sector address (LBA) */
UINT count /* Number of sectors to write (1..128) */
) {
	DSTATUS status;

	if ((pdrv != 0 && pdrv != 1) || !count)
		return RES_PARERR;

	status = disk_status(pdrv);
	if (status & STA_PROTECT)
		return RES_WRPRT;

	uint32_t endAddress = 0, address = 0;
	uint32_t toWrite = 0;
	address = get_address(pdrv, sector);
	endAddress = get_address(pdrv, sector + count);

	do {
		toWrite = byte_to_uint32(buff);
		FLASH_Unlock();
		status = FLASH_ProgramWord((uint32_t) address, toWrite);

		FLASH_Lock();

		if (status != FLASH_COMPLETE)
			return status;

		address += 4;
		buff += 4;
	} while (address < endAddress); //Hasta fin del sector

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(int pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {
	if (pdrv != 0 && pdrv != 1)
		return RES_PARERR;

	uint32_t address;
	DRESULT state;

	switch (cmd) {
	case CTRL_SYNC:
		state = FLASH_WaitForLastOperation();

		return flashState2FSState(state);
		break;

	case GET_SECTOR_COUNT:
		*(DWORD*) buff = (DWORD) FS_SIZE;

		return RES_OK;
		break;

	case GET_SECTOR_SIZE:
		*(DWORD*) buff = SECTOR_SIZE;

		return RES_OK;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*) buff = 1;

		return RES_OK;
		break;

	case CTRL_ERASE_SECTOR:
		address = get_address(pdrv, (int) buff);
		FLASH_Unlock();
		state = flashState2FSState(
				FLASH_EraseSector(GetSector(address), VoltageRange_3));
		FLASH_Lock();

		return state;
		break;
	}

	return RES_PARERR;
}
