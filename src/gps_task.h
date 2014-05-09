#ifndef CONSOLE_TASK_H_
#define CONSOLE_TASK_H_

#include "FreeRTOS.h"
#include "common.h"

void GPSHardwareInit(void *pParam);
void GPSStartTask(unsigned short nStackDepth, unsigned portBASE_TYPE nPriority, void *pParams);

#endif /* CONSOLE_TASK_H_ */
