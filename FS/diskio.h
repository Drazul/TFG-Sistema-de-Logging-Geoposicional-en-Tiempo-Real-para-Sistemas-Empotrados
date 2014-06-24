/**
 @file diskio.h
 @brief Cabecera de la capa de abstracción de operaciones a bajo nivel sobre la memoria
*/
#include "integer.h"
#include "ffconf.h"

/* Status of Disk Functions */
typedef BYTE DSTATUS;

/**< Enumeración de los estados de operación */
typedef enum {
  RES_OK = 0, /* 0: Successful */
  RES_ERROR,  /* 1: R/W Error */
  RES_WRPRT,  /* 2: Write Protected */
  RES_NOTRDY, /* 3: Not Ready */
  RES_PARERR  /* 4: Invalid Parameter */
} DRESULT;

/*---------------------------------------*/
/* Prototypes for disk control functions */

/**
 * @brief Función que incializa la memoria FLASH
 * @param pdrv Identificador del sector físico utilizado
 * @return Código de estado de la operación
 */
DSTATUS disk_initialize(int pdrv);

/**
 * @brief Función que devuelve el estado de la FLASH
 * @param pdrv Identificador del sector físico utilizado
 * @return Estado de la FLASH
 */
DSTATUS disk_status(int pdrv);

/**
 * @brief Función que lee datos desde la memoria FLASH
 * @param pdrv Identificador del sector físico utilizado
 * @param buff Buffer donde devolver los datos leídos
 * @param sector Número de sector lógico donde empezar a leer
 * @param count Número de sectores lógicos a leer
 * @return Código de estado de la operación
 */
DRESULT disk_read(int pdrv, BYTE* buff, DWORD sector, UINT count);

/**
 * @brief Función que escribe en la memoria FLASH
 * @param pdrv Identificador del sector físico utilizado
 * @param buff Stream de datos que se van a escribir
 * @param sector Número de sector lógico donde escribir
 * @param count Número de sectores lógicos a escribir
 * @return Código de estado de la operación
 */
DRESULT disk_write(int pdrv, const BYTE* buff, DWORD sector, UINT count);

/**
 * @brief Función de control de entrada/salida, varias funciones
 * @param pdrv Identificador del sector físico utilizado
 * @param cmd Comando que especifica qué función realizar
 * @param buff Parámetro de entrada o salida dependiendo de la función a realizar
 * @return Código de estado de la operación
 *
 * @verbatim A continuación se especifican todos los comandos disponibles:
    CTRL_SYNC espera a que termine la última operación de la FLASH
    GET_SECTOR_COUNT Devuelve el número de sectores del sistema de archivos
    GET_SECTOR_SIZE Devuelve el tamaño de cada sector lógico
    GET_BLOCK_SIZE Devuelve el número de sectores físicos del sistema de archivos
    CTRL_ERASE_SECTOR Resetea el sector especificado en buff
   @endverbatim
 */
DRESULT disk_ioctl(int pdrv, BYTE cmd, void* buff);
uint32_t GetSector(uint32_t Address);

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT    0x01  /* Drive not initialized */
#define STA_NODISK    0x02  /* No medium in the drive */
#define STA_PROTECT    0x04  /* Write protected */

/* Command code for disk_ioctrl function */
#define CTRL_SYNC      0  /* Wait for last operation */
#define GET_SECTOR_COUNT  1  /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE    2  /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE    3  /* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR  4  /* Force erased a block of sectors (for only _USE_ERASE) */
