#ifndef __BA_BOARD_H
#define __BA_BOARD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f2xx.h"
#include "buffer.h"

#define LEDS_GPIO_PORT				GPIOE
#define LEDS_GPIO_CLK				RCC_AHB1Periph_GPIOE
#define LEDR_PIN					GPIO_Pin_2
#define LEDR_PIN_SOURCE				GPIO_PinSource2
#define LEDV_PIN					GPIO_Pin_3
#define LEDV_PIN_SOURCE				GPIO_PinSource3
#define LEDA_PIN					GPIO_Pin_4
#define LEDA_PIN_SOURCE				GPIO_PinSource4

#define CAN1_CLK					RCC_APB1Periph_CAN1
#define CAN1_GPIO_AF				GPIO_AF_CAN1
#define CAN1_TX_PIN					GPIO_Pin_9
#define CAN1_TX_PIN_SOURCE			GPIO_PinSource9
#define CAN1_TX_GPIO_PORT			GPIOB
#define CAN1_TX_GPIO_CLK			RCC_AHB1Periph_GPIOB
#define CAN1_RX_PIN					GPIO_Pin_8
#define CAN1_RX_PIN_SOURCE			GPIO_PinSource8
#define CAN1_RX_GPIO_PORT			GPIOB
#define CAN1_RX_GPIO_CLK		  	RCC_AHB1Periph_GPIOB
#define CAN1_STANDBY_PIN			GPIO_Pin_8
#define CAN1_STANDBY_PIN_SOURCE		GPIO_PinSource8
#define CAN1_STANDBY_GPIO_PORT		GPIOA
#define CAN1_STANDBY_GPIO_CLK		RCC_AHB1Periph_GPIOA

#define DBG                       UART4
#define DBG_CLK                   RCC_APB1Periph_UART4
#define DBG_GPIO_AF               GPIO_AF_UART4
#define DBG_TX_PIN                GPIO_Pin_10
#define DBG_TX_PIN_SOURCE         GPIO_PinSource10
#define DBG_TX_GPIO_PORT          GPIOC
#define DBG_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define DBG_RX_PIN                GPIO_Pin_11
#define DBG_RX_PIN_SOURCE         GPIO_PinSource11
#define DBG_RX_GPIO_PORT          GPIOC
#define DBG_RX_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define DBG_IRQn                  UART4_IRQn

#define BT                        UART5
#define BT_CLK                    RCC_APB1Periph_UART5
#define BT_GPIO_AF                GPIO_AF_UART5
#define BT_TX_PIN                 GPIO_Pin_12
#define BT_TX_PIN_SOURCE          GPIO_PinSource12
#define BT_TX_GPIO_PORT           GPIOC
#define BT_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define BT_RX_PIN                 GPIO_Pin_2
#define BT_RX_PIN_SOURCE          GPIO_PinSource2
#define BT_RX_GPIO_PORT           GPIOD
#define BT_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define BT_IRQn                   UART5_IRQn
#define BT_RESET_PIN              GPIO_Pin_15
#define BT_RESET_GPIO_PORT        GPIOD
#define BT_RESET_GPIO_CLK         RCC_AHB1Periph_GPIOD

#define GSM					  		USART2
#define GSM_CLK	 			       	RCC_APB1Periph_USART2
#define GSM_GPIO_AF				   	GPIO_AF_USART2
#define GSM_TX_PIN                 	GPIO_Pin_2
#define GSM_TX_PIN_SOURCE          	GPIO_PinSource2
#define GSM_TX_GPIO_PORT           	GPIOA
#define GSM_TX_GPIO_CLK            	RCC_AHB1Periph_GPIOA
#define GSM_RX_PIN                 	GPIO_Pin_3
#define GSM_RX_PIN_SOURCE          	GPIO_PinSource3
#define GSM_RX_GPIO_PORT           	GPIOA
#define GSM_RX_GPIO_CLK            	RCC_AHB1Periph_GPIOA
#define GSM_IRQn                   	USART2_IRQn
#define GSM_RESET_PIN              	GPIO_Pin_12
#define GSM_RESET_GPIO_PORT        	GPIOD
#define GSM_RESET_GPIO_CLK         	RCC_AHB1Periph_GPIOD
#define GSM_IGTCPU_PIN				GPIO_Pin_14
#define GSM_IGTCPU_GPIO_PORT	   	GPIOD


// GPS: USART 3: PB10 (Tx)   PB11(Rx)    PE12(3DFix)
#define GPS					  		USART3
#define GPS_CLK	 			       	RCC_APB1Periph_USART3
#define GPS_GPIO_AF				   	GPIO_AF_USART3
#define GPS_TX_PIN                 	GPIO_Pin_10
#define GPS_TX_PIN_SOURCE          	GPIO_PinSource10
#define GPS_TX_GPIO_PORT           	GPIOB
#define GPS_TX_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define GPS_RX_PIN                 	GPIO_Pin_11
#define GPS_RX_PIN_SOURCE          	GPIO_PinSource11
#define GPS_RX_GPIO_PORT           	GPIOB
#define GPS_RX_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define GPS_IRQn                   	USART3_IRQn
#define GPS_3DFIX_PIN              	GPIO_Pin_12
#define GPS_3DFIX_GPIO_PORT        	GPIOE
#define GPS_3DFIX_GPIO_CLK         	RCC_AHB1Periph_GPIOE


// GPS


typedef enum {
 	COM_DBG = 0,
 	COM_BT = 1,
	COM_GSM = 2,
	COM_GPS = 3
 } COM_TypeDef;

 typedef enum {
	 BLOCKING_TRANSFER,
     NON_BLOCKING_TRANSFER
   } Transfer_TypeDef;

 typedef enum {
     RX_TIMER,
     TX_TIMER
 } Timer_TypeDef;

 typedef enum {
	 IDLE,
	 BUSY
 } CommState_TypeDef;

 typedef struct {
	 uint8_t *txBuffer;
	 uint16_t txSize;
	 uint16_t txWrPtr;
	 CommState_TypeDef txState;
	 uint8_t *rxBuffer;
	 uint16_t rxSize;
	 CommState_TypeDef rxState;
 } CommControl_TypeDef;

void COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
COM_TypeDef getCOMFromUSART(USART_TypeDef* USARTx);
//void USART_SetTimer(USART_TypeDef* USARTx, uint16_t ticks, Timer_TypeDef timerType);
//void USART_EnableTimer(USART_TypeDef* USARTx, Timer_TypeDef timerType);

void CANInit();
void BA_LEDsInit();
void BA_SetLED(const uint16_t led);
void BA_GetLED(const uint16_t led);

uint16_t USART_Send(USART_TypeDef* USARTx, uint8_t *msg, uint16_t size);
void USART_Send_NBK(COM_TypeDef com, uint8_t *msg, uint16_t size);
uint16_t USART_Receive(USART_TypeDef* USARTx, uint8_t *msg, uint16_t size);
void USART_Receive_NBK(COM_TypeDef com, uint8_t *msg, uint16_t size);

void BA_SystimerInit();
void Delay(uint32_t nTime);
void EnableCOMDelay(uint32_t nTime);
void BA_NVICInit(COM_TypeDef com);


void BA_DBGInit(USART_InitTypeDef* USART_InitStruct);
uint16_t BA_DBGSend(uint8_t *msg);
void BA_DBGSend_NBK(uint8_t *msg);
uint16_t BA_DBGReceive(uint8_t *msg, uint16_t size);

void BA_BTInit(USART_InitTypeDef* USART_InitStruct);
void BA_BTSend(uint8_t *msg, unsigned int size);
void BA_BTSend_NBK(uint8_t *msg);
void BA_BT_SendFromIRQ();
uint16_t BA_BTReceive(uint8_t *msg, uint16_t size);
void BA_BT_Receive_NBK(uint8_t *msg, uint16_t size);
void BA_BT_ReceiveFromIRQ();
uint8_t BA_BT_isRxBusy();

void BA_GSMInit(USART_InitTypeDef* USART_InitStruct);
void BA_GSMSend(uint8_t *msg, uint16_t size);
uint16_t BA_GSMReceive(uint8_t *msg, uint16_t size);
void BA_GSMRestart();


// GPS
void BA_GPSInit(USART_InitTypeDef* USART_InitStruct);
void BA_GPSSend(uint8_t *msg, uint16_t size);
uint16_t BA_GPSReceive(uint8_t *msg, uint16_t size);


#ifdef __cplusplus
}
#endif


#endif /* __STM32_EVAL_H */
