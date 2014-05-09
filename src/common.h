#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f2xx.h"

xQueueHandle WriteQueue;

typedef struct {
	uint16_t count;
	uint8_t buffer[80];
}GPS_MSG;
