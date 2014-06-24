/**
 @file gps_task.c
 @brief Módulo que implementa las funciones del archivo gps_task.h
*/
#include "gps_task.h"
#include "stm32f2xx.h"
#include "BA_board.h"
#include "task.h"
#include <stdlib.h>

//#define COMn 4
//USART_TypeDef* COM_USART[COMn] = {DBG, BT, GSM, GPS};
/**
 * @brief Esta es la función principal de la tarea
 * @Note Esta función inicializa el GPS y lanza el método que lee datos del GPS y se los manda a la tarea que gestiona el Sistema de Archivos
 */
void GPSTaskFunc(void *pParams);

/**
 * @brief Esta es la función que inicializa el GPS
 */
void setupGPS() {
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl =
  USART_HardwareFlowControl_None;

  BA_GPSInit(&USART_InitStructure);
  Delay(1000);
}
/**
 * @brief Esta es la función que inicializa todo el hardware que necesita la tarea
 */
void GPSHardwareInit(void *pParam) {
  setupGPS();
}

void GPSStartTask(unsigned short nStackDepth, unsigned portBASE_TYPE nPriority,
    void *pParams) {
  xTaskCreate(GPSTaskFunc, "GPS", nStackDepth, pParams, nPriority, NULL);
}

/**
 * @brief Esta es la función que lee datos del GPS y, cuando detecta un mensaje correcto, lo envía a la tarea que gestiona el Sistema de Archivos
 * @Note Esta tarea enciende un led cada vez que envía correctamente un mensaje
 */
void parser_GPS() {
  uint8_t answer[80];
  uint16_t parser_idx = 0;
  uint8_t c;
  int state = 0;  // 0-5 -> searching for $GPGGA header,  1-> getting data
  GPS_MSG msg;
  int i;
  // Parse for $GPGGA statements
  // $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65\r

  while (1) {
    GPIO_ResetBits(LEDS_GPIO_PORT, LEDV_PIN);
    // BA_DBGSend((uint8_t*)"\r\n New GPGGA reading ... \r\n");
    while (state < 7) {
      BA_GPSReceive(&c, 1);

      switch (state) {
      case 0:
        if (c == '$') {
          answer[parser_idx] = c;
          state = 1;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 1:
        if (c == 'G') {
          answer[parser_idx] = c;
          state = 2;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 2:
        if (c == 'P') {
          answer[parser_idx] = c;
          state = 3;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 3:
        if (c == 'G') {
          answer[parser_idx] = c;
          state = 4;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 4:
        if (c == 'G') {
          answer[parser_idx] = c;
          state = 5;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 5:
        if (c == 'A') {
          answer[parser_idx] = c;
          state = 6;
          parser_idx++;
        } else {
          parser_idx = 0;
          state = 0;
        }
        break;

      case 6: // get the rest of the message
        while (c != '\r') {
          BA_GPSReceive(&c, 1);
          answer[parser_idx] = c;
          parser_idx++;
        }
        //BA_GPSReceive(&(answer[parser_idx]), 60);
        state = 7;
        break;

      default:
        // get the rest of the message
        break;
      }

    }

    answer[parser_idx] = '\n';
    for (i=0;i<80;i++){
      msg.buffer[i]=answer[i];
    }
    //msg.buffer=(uint8_t[80])answer;
    msg.count=parser_idx;

    xQueueSend(writeQueue, &msg, 1000);
    GPIO_SetBits(LEDS_GPIO_PORT, LEDV_PIN);

    Delay(1000);

    state = 0;
    parser_idx = 0;
  }
}

void GPSTaskFunc(void *pParams) {

  GPSHardwareInit(pParams);
  Delay(2000);

  while (1) {
    parser_GPS();
  }

}
