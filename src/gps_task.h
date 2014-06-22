/**
 @file gps_task.h
 @brief Cabecera de la tarea que gestiona el GPS
*/

#ifndef CONSOLE_TASK_H_
#define CONSOLE_TASK_H_

#include "FreeRTOS.h"
#include "common.h"

/**
 * @brief Método que inicializa el hardware de la tarea
 */
void GPSHardwareInit(void *pParam);
/**
 * @brief Método que crea la tarea
 * @param sStackDepth Tamaño de la pila de memoria
 * @param nPrioriy Prioridad de la tarea
 */
void GPSStartTask(unsigned short nStackDepth, unsigned portBASE_TYPE nPriority, void *pParams);

#endif /* CONSOLE_TASK_H_ */
