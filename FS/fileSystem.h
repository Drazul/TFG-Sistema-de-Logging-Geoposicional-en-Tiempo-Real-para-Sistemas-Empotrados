/**
 @file fileSystem.h
 @brief Cabecera principal del módulo del Sistema de Archivos
*/

#include "integer.h"
#include "stm32f2xx.h"
#include "diskio.h"

/* FILE PERMISSIONS */
#define   FA_READ      0x01
#define   FA_OPEN_EXISTING  0x00

#define  FA_WRITE      0x02
#define   FA_CREATE_NEW    0x04
#define   FA_CREATE_ALWAYS  0x08
#define   FA_OPEN_ALWAYS    0x10

/**
 *  @brief Enumeración de los estados de operación de la memoria FLASH
 */
typedef enum {
  FR_OK = 0, /**< (0) Éxito */
  FR_DISK_ERR, /**< (1) Ha ocurrido un error en la capa de abstracción de E/S */
  FR_INT_ERR, /**< (2) Parámetros erróneos */
  FR_NOT_READY, /**< (3) Memoria ocupada */
  FR_NO_FILE, /**< (4) No se encuentra el archivo */
  FR_NO_PATH, /**< (5) No se encuentra la ruta */
  FR_INVALID_NAME, /**< (6) El nombre o la ruta es inválido */
  FR_DENIED, /**< (7) Acceso denegado o prohibido el acceso al directorio */
  FR_EXIST, /**< (8) Acceso denegado o prohibido el acceso */
  FR_INVALID_OBJECT, /**< (9) El archivo o directorio es inválido */
  FR_WRITE_PROTECTED, /**< (10) La memoria está protegida contra escrituras */
  FR_INVALID_DRIVE, /**< (11) El identificador del medio físco es inválido */
  FR_NOT_ENABLED, /**< (12) El volúmen no tiene área de trabajo */
  FR_NO_FILESYSTEM, /**< (13) No hay un Sistema de Archivos válido */
  FR_MKFS_ABORTED, /**< (14) Abortada la creación de un Sistema de Archivos debido a parámetros erróneos */
  FR_TIMEOUT, /**< (15) Tiempo de espera excedido */
  FR_LOCKED, /**< (16) La operación ha sido rechazada debido a las políticas de acceso */
  FR_NOT_ENOUGH_CORE, /**< (17) No hay espacio disponible para guardar */
  FR_TOO_MANY_FILES, /**< (18) Número de archivos máximo excedido */
  FR_INVALID_PARAMETER /**< (19) El parámetro dado es inválido */
} FRESULT;

/**
 *  @brief Estructura que define un archivo
 */typedef struct {
  BYTE name[7]; /**< Nombre del archivo */
  BYTE flag; /**< Estado y permisos del archivo */
  BYTE err; /**< Estado de error */
  BYTE dirty; /**< Si hay operaciones de escritura pendientes*/
  BYTE fsIndex; /**< Índice en el sistema de archivos */
  DWORD startSector; /**< Sector de inicio del archivo */
  DWORD writePointer; /**< Puntero de escritura del archivo. Se encuentra al final del archivo. En bytes */
  DWORD readPointer; /**< Puntero de lectura. En Bytes */
  DWORD descriptorSector; /**< Sector del sistema de archivos en el que se encuentra el archivo */
  WORD buffindex; /**< Índice del buffer donde se ha escrito por última vez */
  BYTE buff[SECTOR_SIZE];/**< buffer de escritura */
} FIL;

/**
 *  @brief Estructura que define el Sistema de Archivos
 */
typedef struct {
  uint32_t start_address; /**< Dirección de inicio del sistema de archivos */
  BYTE sector_size; /**< Tamaño de cada sector lógico */
  WORD free_sector; /**< Número de sectores libres */
  WORD fs_size; /**< Tamaño en sectores lógicos del sistema de archivos */
  WORD volbase; /**< Sector de inicio del volúmen de datos */
  WORD fatbase; /**< Sector de inicio del sistema de archivos */
  WORD database; /**< Sector de inicio de la tabla de descriptores */
  WORD lastDescriptor; /**< Sector donde se guardó el último descriptor de archivo */
  FIL files[_MAX_FILES]; /**< Vector de archivos abiertos */
  BYTE win[SECTOR_SIZE]; /**< Buffer de lectura del sistema de archivos y de los archivos */
} FS;

/**
 * @brief Función que abre o crea un archivo
 * @param fp Parámetro de entrada en el que se devolverá el puntero del archivo abierto
 * @param path Nombre del archivo que queremos abrir
 * @param mode Permisos de apertura del archivo
 * @return Código de estado de la operación
 */
FRESULT f_open(FIL* fp, const TCHAR* path, BYTE mode);

/**
 * @brief Función que cierra un archivo
 * @param fp Parámetro de entrada en el que se especifica el fichero a cerrar
 * @return Código de estado de la operación
 */
FRESULT f_close(FIL* fp); /* Close an open file object */

/**
 * @brief Función que lee de un archivo
 * @param fp Parámetro de entrada en el que se especifica el fichero de donde leer
 * @param buff Buffer donde se devolverán los datos leídos
 * @param btr Dirección de donde empezar a leer
 * @param br Cantidad de bytes a leer
 * @return Código de estado de la operación
 */
FRESULT f_read(FIL* fp, void* buff, UINT btr, UINT* br); /* Read data from a file */

/**
 * @brief Función que escribe de un archivo
 * @param fp Parámetro de entrada en el que se especifica el fichero de donde leer
 * @param buff Stream de datos que se quieren escribir
 * @param btw Dirección de donde empezar a escribir
 * @param bw Cantidad de bytes a leer
 * @return Código de estado de la operación
 */
FRESULT f_write(FIL* fp, const void* buff, UINT btw, UINT* bw); /* Write data to a file */

/**
 * @brief Función que elimina parte de un archivo
 * @param fp Parámetro de entrada en el que se especifica el fichero objetivo
 * @param ofs Cantidad de bytes a eliminar desde la posición cero del archivo
 * @return Código de estado de la operación
 */
FRESULT f_truncateStart(FIL* fp, DWORD ofs); /* Move start file pointer of a file object */

/**
 * @brief Función que mueve el puntero de lectura de un archivo
 * @param fp Parámetro de entrada en el que se especifica el fichero objetivo
 * @param ofs Cantidad de bytes a mover desde la posición actual del puntero de lectura
 * @return Código de estado de la operación
 */
FRESULT f_lseek(FIL* fp, DWORD ofs); /* Move read pointer of a file object  */

/**
 * @brief Función que sincroniza un archivo, escribe en memoria los datos que queden pendientes
 * @param fp Parámetro de entrada en el que se especifica el fichero objetivo
 * @return Código de estado de la operación
 */
FRESULT f_sync(FIL* fp); /* Flush cached data of a writing file */

/**
 * @brief Función que devuelve el número de sectores libres del sistema de archivos
 * @param fsIndex Identificador del sistema de archivos
 * @param nclst Puntero donde se devuelve el valor
 * @return Código de estado de la operación
 */
FRESULT f_getfree(int fsIndex, UINT* nclst); /* Get number of free clusters on the drive */

/**
 * @brief Función que crea la estructura necesaria para crear un sistema de archivos
 * @param fileSystem Puntero de Sistema de Archivos donde devolver el valor
 * @param fsIndex Identificador del sistema de archivos objetivo
 * @param opt Acepta dos opciones: 0, sólo crea la estructura necesaria para crear un sistema de archivos. 1, intenta montar un sistema de archivos existente
 * @return Código de estado de la operación
 */
FRESULT f_mount(FS* fileSystem, int fsIndex, BYTE opt); /* Mount/Unmount a logical drive */

/**
 * @brief Función que crea un sistema de archivos
 * @param fsIndex Identificador del sistema de archivos a crear
 * @return Código de estado de la operación
 */
FRESULT f_mkfs(int fsIndex); /* Create a file system on the volume */

/**
 * @brief Función que resetea un sector
 * @param fsIndex Identificador del sistema de archivos sobre el que resetear sus sectores
 * @return Código de estado de la operación
 */
FRESULT reset_sector(int fsIndex);
