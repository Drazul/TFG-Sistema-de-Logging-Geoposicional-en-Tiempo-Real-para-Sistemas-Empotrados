/**
 @file common.h
 @brief En este archivo se definen estructuras de datos comunes para todas las tareas.
*/
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f2xx.h"

xQueueHandle writeQueue;
/**
 *  @brief Estructura que define el mensaje de la cola de mensaje writeQueue
 */
typedef struct {
	uint16_t count; /**<  Número de caracteres del mensaje enviado */
	uint8_t buffer[80];/**<  Buffer donde se almacena el mensaje*/
}GPS_MSG;
