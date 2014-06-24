/**
 @file fs_task.c
 @brief Módulo que implementa las funciones del archivo fs_task.h
*/
#include "fs_task.h"
#include "stm32f2xx.h"
#include "BA_board.h"
#include "task.h"
#include <stdlib.h>
#include "fileSystem.h"

static FS fileSystem;

/**
 * @brief Esta es la función principal de la tarea
 * @Note Esta función inicializa la memoria y el sistema de archivos y lanza el método que se queda a la espera de mensajes desde el GPS
 */
void FSTaskFunc(void *pParams);

void FSHardwareInit(void *pParam) {
  reset_sector(0);
  f_mount(&fileSystem, 0, 0);
  f_mkfs(0);
  f_mount(&fileSystem, 0, 1);
}

void FSStartTask(unsigned short nStackDepth, unsigned portBASE_TYPE nPriority,
    void *pParams) {
  xTaskCreate(FSTaskFunc, "FS", nStackDepth, pParams, nPriority, NULL);
}

/**
 * @brief Función que lee de un archivo
 * @Note Esta función no se utiliza en la implementación actual, pero se mantiene para poder ser utilizada cuando se integre en el proyecto Biker Assistant
 */
void read_file() {
  FIL fp;
  GPS_MSG msg;
  f_open(&fp, "ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  int i;
  UINT readed = 0;
  for (i = 0; i < 10; i++) {
    f_read(&fp, &msg.buffer, 37, &readed);
  }
}

// FS
/*
void test_FS() {
  FIL fp;

  f_open(&fp, "ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

  uint8_t toWrite[100];
  uint8_t toRead[100];
  int i;
  UINT writed, readed;
  for (i = 0; i < 100; i++) {
    toWrite[i] = '2';
    toRead[i] = '1';
  }

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

  for (i = 0; i < 100; i++) {
    f_write(&fp, toWrite, 100, &writed);
  }

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

  f_close(&fp);

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

  f_open(&fp, "ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  for (i = 0; i < 100; i++) {
    f_read(&fp, toRead, 100, &readed);
  }

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

  f_close(&fp);

  Delay(500);
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  Delay(500);
  GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);
}
*/

/**
 * @brief Función que escribe en un archivo todos los mensajes recibidos desde la tarea que controla el GPS
 * @Note Esta función, dentro del bucle de ejecución, enciende y apaga un led cada vez que recibe y escribe un dato
 */
void ReceiveWriteGPS(){
  int count =10;
  UINT writed;
  GPS_MSG msg;
  FIL fp;
  f_open(&fp, "ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  while (count >0){
    Delay(500);
    GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);

    count--;
    xQueueReceive(writeQueue, &msg, 10000);
    f_write(&fp, msg.buffer, msg.count, &writed);

    Delay(500);
    GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
  }
  f_close(&fp);
}


void FSTaskFunc(void *pParams) {

  FSHardwareInit(pParams);
  Delay(2000);

  while (1) {
    //test_FS();
    ReceiveWriteGPS();
    //read_file();
  }

}
