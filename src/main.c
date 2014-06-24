/**
 @file main.c
 @brief Función principal
*/

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

/**
 * @brief Función que inicializa el hardware
 * @note Esta función inicializa el vector de interrupciones, configura los niveles de priporidad, establece la fuente del reloj del sistema e inicializa los leds
 */
void prvSetupHardware( void );

/**
 * External dependence needed by printf implementation. Write a character to standard out.
 *
 * @param c Specifies the character to be written.
 * @return Returns the character written. No error conditions are managed.
 */
int putChar( int ch );

/**
 * @brief Función que crea las tareas del Sistemas Operativo FreeRTOS
 */
void setupTasks() {
  GPSStartTask(configMINIMAL_STACK_SIZE*4, 1, NULL);
  FSStartTask(configMINIMAL_STACK_SIZE*4, 1, NULL);
}
/**
 * @brief Función que crea las colas de mensajes entre tareas del Sistema Operativo FreeRTOS
 */
void setupQueues(){
  writeQueue = xQueueCreate(4, sizeof(GPS_MSG));
}

/*-----------------------------------------------------------*/
/**
 * @brief Función principal del sistema
 * @note Esta función inicializa el hardware, crea las colas de mensajes, crea las tareas e inicializa el planificador del Sistema Operativo
 */
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

void prvSetupHardware( void )
{
  /* Set the Vector Table base address at 0x08000000 */
  NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
  BA_LEDsInit();

}

/*-----------------------------------------------------------*/
/**
 * @brief Función que crea la excepción de desbordamiento de la pila de memoria
 * @note Esta función la llama internamente el Sistema Operativo
 */
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
/**
 * @brief Función que crea la excepción del reloj
 * @note Esta función la llama internamente el Sistema Operativo
 */
void vApplicationTickHook( void )
{
}

/*-----------------------------------------------------------*/

int putChar(int ch)
{
  return ch;
}

/**
 * @brief Función que define la función de espera
 * @param t Tiempo a esperar en milisegundos
 */
void Delay(uint32_t t) {
  vTaskDelay(t / portTICK_RATE_MS);
}
