/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "stm32f2xx.h"
#include "BA_board.h"

/* Demo app includes. */
#include "gps_task.h"
#include "fs_task.h"


/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/**
 * External dependence needed by printf implementation. Write a character to standard out.
 *
 * @param c Specifies the character to be written.
 * @return Returns the character written. No error conditions are managed.
 */
int putChar( int ch );

void setupTasks() {
	GPSStartTask(configMINIMAL_STACK_SIZE*4, 1, NULL);
	FSStartTask(configMINIMAL_STACK_SIZE*4, 1, NULL);
}

void setupQueues(){
	WriteQueue = xQueueCreate(4, sizeof(GPS_MSG));
}

/*-----------------------------------------------------------*/

int main( void )
{
	prvSetupHardware();
	setupQueues();
	setupTasks();


    /* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	for( ;; );

	return 0;
}
/*-----------------------------------------------------------*/


static void prvSetupHardware( void )
{
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
	BA_LEDsInit();

}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
}

/*-----------------------------------------------------------*/

int putChar(int ch)
{
	return ch;
}

void Delay(uint32_t t) {
	vTaskDelay(t / portTICK_RATE_MS);
}
