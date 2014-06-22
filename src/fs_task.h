/**
 @file fs_task.h
 @brief Cabecera de la tarea que gestiona el sistema de archivos
*/
#ifndef CONSOLE_TASK_H_
#define CONSOLE_TASK_H_

#include "FreeRTOS.h"
#include "common.h"

/**
 * @brief Método que inicializa el hardware de la tarea
 * @Note Esta función resetea un sector y crea en él un sistema de archivos
 */
void FSHardwareInit(void *pParam);
/**
 * @brief Método que crea la tarea
 * @param sStackDepth Tamaño de la pila de memoria
 * @param nPrioriy Prioridad de la tarea
 */
void FSStartTask(unsigned short nStackDepth, unsigned portBASE_TYPE nPriority, void *pParams);

#endif /* CONSOLE_TASK_H_ */
