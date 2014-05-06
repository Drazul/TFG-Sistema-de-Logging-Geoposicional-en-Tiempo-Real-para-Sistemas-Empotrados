#include <string.h>
#include <stdlib.h>
#include "BA_board.h"

#define COMn 4

USART_TypeDef* COM_USART[COMn] = {DBG, BT, GSM, GPS};
const uint32_t COM_USART_CLK[COMn] = {DBG_CLK, BT_CLK, GSM_CLK, GPS_CLK};
const uint32_t COM_USART_AF[COMn] = {DBG_GPIO_AF, BT_GPIO_AF, GSM_GPIO_AF, GPS_GPIO_AF};

GPIO_TypeDef* COM_TX_PORT[COMn] = {DBG_TX_GPIO_PORT, BT_TX_GPIO_PORT, GSM_TX_GPIO_PORT, GPS_TX_GPIO_PORT };
const uint32_t COM_TX_PORT_CLK[COMn] = {DBG_TX_GPIO_CLK, BT_TX_GPIO_CLK, GSM_TX_GPIO_CLK, GPS_TX_GPIO_CLK };
const uint16_t COM_TX_PIN[COMn] = {DBG_TX_PIN, BT_TX_PIN, GSM_TX_PIN, GPS_TX_PIN};
const uint16_t COM_TX_PIN_SOURCE[COMn] = {DBG_TX_PIN_SOURCE, BT_TX_PIN_SOURCE, GSM_TX_PIN_SOURCE, GPS_TX_PIN_SOURCE};

GPIO_TypeDef* COM_RX_PORT[COMn] = {DBG_RX_GPIO_PORT, BT_RX_GPIO_PORT, GSM_RX_GPIO_PORT, GPS_RX_GPIO_PORT};
const uint32_t COM_RX_PORT_CLK[COMn] = {DBG_RX_GPIO_CLK, BT_RX_GPIO_CLK, GSM_RX_GPIO_CLK, GSM_RX_GPIO_CLK};
const uint16_t COM_RX_PIN[COMn] = {DBG_RX_PIN, BT_RX_PIN, GSM_RX_PIN, GPS_RX_PIN};
const uint16_t COM_RX_PIN_SOURCE[COMn] = {DBG_RX_PIN_SOURCE, BT_RX_PIN_SOURCE, GSM_RX_PIN_SOURCE, GPS_RX_PIN_SOURCE};

static CommControl_TypeDef COM_CONTROL[COMn];
static BufferType COM_RX_FIFO[COMn];

volatile static uint32_t TimingDelay ;
volatile static uint32_t COMDelay ;
uint8_t COMDelayEnabled = 0;

RCC_ClocksTypeDef RCC_Clocks;

/* Función que configura el Systick a 1ms */
void BA_SystimerInit(void) {
	RCC_GetClocksFreq(&RCC_Clocks) ;
	SysTick_Config(RCC_Clocks.HCLK_Frequency/1000) ;
}
/* Función para implementar delays de nTime ms */

void Delay(uint32_t nTime) {
	TimingDelay = nTime;
	while (TimingDelay != 0);
}

void EnableCOMDelay(uint32_t nTime) {
  COMDelay = nTime;
  COMDelayEnabled = 1;
}

void DisableCOMDelay() {
  COMDelayEnabled = 0;
}

/* Función que será llamada cada vez que salte la interrupción del SysTick
*/

void TimingDelay_Decrement (void) {
	if (TimingDelay != 0x00) {
		TimingDelay --;
	}
	if (COMDelayEnabled && COMDelay != 0)
	  COMDelay--;
}

void COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO and peripheral clocks */
	RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);
	RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);

	/* Configure alternate GPIO pins */
	GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_PIN_SOURCE[COM], COM_USART_AF[COM]);
	GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_USART_AF[COM]);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
	GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

	/* Enable Reset for the Bluetooth peripheral */
	if (COM == COM_BT) {
		int i;
		RCC_APB1PeriphClockCmd(BT_RESET_GPIO_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Pin = BT_RESET_PIN;
		GPIO_Init(BT_RESET_GPIO_PORT, &GPIO_InitStructure);
		GPIO_ResetBits(BT_RESET_GPIO_PORT, BT_RESET_PIN);
		for (i = 0; i< 0x100000; i++);
		//Delay(200); // FIXME: blocks freeRTOS
		GPIO_SetBits(BT_RESET_GPIO_PORT, BT_RESET_PIN);
	}

	/* USART configuration */
	if (COM == COM_GSM) {
		RCC_APB1PeriphClockCmd(GSM_RESET_GPIO_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Pin = GSM_RESET_PIN;
		GPIO_Init(GSM_RESET_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Pin = GSM_IGTCPU_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GSM_IGTCPU_GPIO_PORT, &GPIO_InitStructure);
	    GPIO_ResetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	}

	// GPS
	if (COM == COM_GPS)
	{
		// Nothing else to do (done in BA_GPSInit).
	}

	/* USART configuration */
	USART_Init(COM_USART[COM], USART_InitStruct);

	/* Enable USART */
	USART_Cmd(COM_USART[COM], ENABLE);

	if (COM == COM_GSM) {
		GPIO_ResetBits(GSM_RESET_GPIO_PORT, GSM_RESET_PIN);
		Delay(300);
		GPIO_SetBits(GSM_RESET_GPIO_PORT, GSM_RESET_PIN);
		Delay(300);
		GPIO_ResetBits(GSM_RESET_GPIO_PORT, GSM_RESET_PIN);
	}

	/* Initialize com driver state data */
	initBuffer(&COM_RX_FIFO[COM]);
	COM_CONTROL[COM].txState = IDLE;
	COM_CONTROL[COM].txSize = 0;
	initBuffer(&COM_RX_FIFO[COM]);
	COM_CONTROL[COM].rxState = IDLE;
	COM_CONTROL[COM].rxSize = 0;
}

COM_TypeDef getCOMFromUSART(USART_TypeDef* USARTx) {
	if (USARTx == USART2) return COM_GSM;
    if (USARTx == UART4) return COM_DBG;
	if (USARTx == UART5) return COM_BT;
	if (USARTx == USART3) return COM_GPS;		// GPS
	return NULL;
}

//   void USART_SetTimer(USART_TypeDef* USARTx, uint16_t ticks, Timer_TypeDef timerType);
//   void USART_EnableTimer(USART_TypeDef* USARTx, Timer_TypeDef timerType);

void CANInit(){
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable CAN GPIOs and peripheral clocks */
	RCC_AHB1PeriphClockCmd(CAN1_TX_GPIO_CLK | CAN1_RX_GPIO_CLK | CAN1_STANDBY_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(CAN1_CLK, ENABLE);

	/* Configure alternate GPIO pins */
	GPIO_PinAFConfig(CAN1_TX_GPIO_PORT, CAN1_TX_PIN_SOURCE, CAN1_GPIO_AF);
	GPIO_PinAFConfig(CAN1_RX_GPIO_PORT, CAN1_RX_PIN_SOURCE, CAN1_GPIO_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = CAN1_TX_PIN;
	GPIO_Init(CAN1_TX_GPIO_PORT, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = CAN1_RX_PIN;
	GPIO_Init(CAN1_TX_GPIO_PORT, &GPIO_InitStructure);

	/* Configure STANDBY pin as output */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(CAN1_STANDBY_GPIO_PORT, &GPIO_InitStructure);

	/* Set no STANDBY */
	GPIO_ResetBits(CAN1_STANDBY_GPIO_PORT, CAN1_STANDBY_PIN);

}

void BA_LEDsInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable LEDs GPIO clock */
	RCC_AHB1PeriphClockCmd(LEDS_GPIO_CLK, ENABLE);

	/* Configure LEDs pins as outputs */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = LEDR_PIN;
	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LEDV_PIN;
	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LEDA_PIN;
	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);

	/* Reset initial value of the LEDs */
	GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
	GPIO_ResetBits(LEDS_GPIO_PORT, LEDV_PIN);
	GPIO_ResetBits(LEDS_GPIO_PORT, LEDA_PIN);
}

void BA_SetLED(const uint16_t led) {
	GPIO_SetBits(LEDS_GPIO_PORT, led);
}

void BA_GetLED(const uint16_t led) {
	// FIXME
}

/* Blocking communication primitives */
uint16_t USART_Send(USART_TypeDef* USARTx, uint8_t *msg, uint16_t size) {

	while(size--) {
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) ;
		USART_SendData(USARTx, (uint16_t) *msg++);
	}
	return size;
}

/* Non-Blocking communication primitives */
inline CommState_TypeDef USART_TxState(COM_TypeDef com) {
	return COM_CONTROL[com].txState;
}

void USART_Send_NBK(COM_TypeDef com, uint8_t *msg, uint16_t size) {

	COM_CONTROL[com].txBuffer = msg;
	COM_CONTROL[com].txSize = size;
	COM_CONTROL[com].txState = BUSY;
	USART_SendData(COM_USART[com],COM_CONTROL[com].txBuffer[0]);
	COM_CONTROL[com].txWrPtr = 1;
	USART_ITConfig(COM_USART[com], USART_IT_TXE, ENABLE);
}

void USART_SendFromIRQ(COM_TypeDef com) {
	if (COM_CONTROL[com].txWrPtr < COM_CONTROL[com].txSize) {
		USART_SendData(COM_USART[com], COM_CONTROL[com].txBuffer[COM_CONTROL[com].txWrPtr++]);
	}
	else {
		COM_CONTROL[com].txState = IDLE;
		USART_ITConfig(COM_USART[com], USART_IT_TXE, ENABLE);
	}
}

uint16_t USART_Receive(USART_TypeDef* USARTx, uint8_t *msg, uint16_t size) {
	int i = 0;
	while (i < size) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
			if (COMDelayEnabled && COMDelay == 0)
				break;

		if (COMDelayEnabled && COMDelay == 0)
			break;
		msg[i++] = USART_ReceiveData(USARTx);
	}
	DisableCOMDelay();
	return i;
}

void USART_Receive_NBK(COM_TypeDef com, uint8_t *msg, uint16_t size) {
	COM_CONTROL[com].rxBuffer = msg;
	COM_CONTROL[com].rxSize = size;
	if (bufferCount(&COM_RX_FIFO[com]) <= size)
		COM_CONTROL[com].rxState = BUSY;
	else {
		// FIXME: reemplazar por memcpy
		int i;
		for (i = 0; i < size; i++)
			msg[i] = bufferDeque(&COM_RX_FIFO[com]);
	}

}

void USART_ReceiveFromIRQ(COM_TypeDef com) {
	int i;

	bufferEnque(&COM_RX_FIFO[com], USART_ReceiveData(COM_USART[com]));

	if (COM_CONTROL[com].rxState == BUSY)
		if (bufferCount(&COM_RX_FIFO[com]) >= COM_CONTROL[com].rxSize) {
			COM_CONTROL[com].rxState = IDLE;
			for (i = 0; i < COM_CONTROL[com].rxSize; i++)
				COM_CONTROL[com].rxBuffer[i] = bufferDeque(&COM_RX_FIFO[com]);
		}
}

/* DBG Peripheral */
void BA_DBGInit(USART_InitTypeDef* USART_InitStruct) {
	COMInit(COM_DBG, USART_InitStruct);
}

uint16_t BA_DBGSend(uint8_t *msg) {
	return USART_Send(COM_USART[COM_DBG], msg, strlen((char*)msg));
}

uint16_t BA_DBGSendSize(uint8_t *msg, uint16_t size) {
	return USART_Send(COM_USART[COM_DBG], msg, size);
}

void BA_DBGSend_NBK(uint8_t *msg) {
	USART_Send_NBK(COM_DBG, msg, strlen((char*)msg));
}

uint16_t BA_DBGReceive(uint8_t *msg, uint16_t size) {
	return USART_Receive(COM_USART[COM_DBG], msg, size);
}

/* BT Peripheral */
void BA_BTInit(USART_InitTypeDef* USART_InitStruct) {
  COMInit(COM_BT, USART_InitStruct);
}

void BA_BTSend(uint8_t *msg, unsigned int size) {
	USART_Send(COM_USART[COM_BT], msg, size);
}

void BA_BTSend_NBK(uint8_t *msg) {
	USART_Send_NBK(COM_BT, msg, strlen((char*)msg));
}

void BA_BT_SendFromIRQ() {
	USART_SendFromIRQ(COM_BT);
}

uint16_t BA_BTReceive(uint8_t *msg, uint16_t size) {
        return USART_Receive(COM_USART[COM_BT], msg, size);
}

void BA_BT_Receive_NBK(uint8_t *msg, uint16_t size) {
	USART_Receive_NBK(COM_BT, msg, size);
}

void BA_BT_ReceiveFromIRQ() {
	USART_ReceiveFromIRQ(COM_BT);
}

uint8_t BA_BT_isRxBusy() {
	return (COM_CONTROL[COM_BT].rxState == BUSY);
}

void BA_GSMPwrOn(){
	GPIO_SetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	Delay (1000);
    GPIO_ResetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
}


void BA_GSMInit(USART_InitTypeDef* USART_InitStruct) {
	COMInit(COM_GSM, USART_InitStruct);
	BA_GSMPwrOn();
	Delay (1000);
}

void BA_GSMRestart() {
	GPIO_SetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	Delay (5000);
    GPIO_ResetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	Delay (1000);  
}

void BA_GSMPwrOff() {
	GPIO_ResetBits(GSM_RESET_GPIO_PORT, GSM_RESET_PIN);
	Delay(500);
	GPIO_SetBits(GSM_RESET_GPIO_PORT, GSM_RESET_PIN);

	GPIO_SetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	Delay (2000);
	GPIO_ResetBits(GSM_IGTCPU_GPIO_PORT, GSM_IGTCPU_PIN);
	Delay (15000);
}

void BA_GSMSend(uint8_t *msg, uint16_t size) {
	USART_Send(COM_USART[COM_GSM], msg, size);
}

uint16_t BA_GSMReceive(uint8_t *msg, uint16_t size) {
	return USART_Receive(COM_USART[COM_GSM], msg, size);
}


// GPS
void BA_GPSInit(USART_InitTypeDef* USART_InitStruct)
{
	COMInit(COM_GPS, USART_InitStruct);
}
void BA_GPSSend(uint8_t *msg, uint16_t size)
{
	USART_Send(COM_USART[COM_GPS], msg, size);
}
uint16_t BA_GPSReceive(uint8_t *msg, uint16_t size)
{
	return USART_Receive(COM_USART[COM_GPS], msg, size);
}
// GPS



void BA_NVICInit(COM_TypeDef com) {
	NVIC_InitTypeDef NVIC_InitStructure;

	switch(com) {
	case COM_DBG:
	  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	  break;
	case COM_BT:
	  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	  break;
	case COM_GSM:
	  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	case COM_GPS:	// GPS
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		break;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

