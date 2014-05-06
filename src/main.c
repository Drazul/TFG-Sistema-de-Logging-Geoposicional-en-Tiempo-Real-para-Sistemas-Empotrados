#include <string.h>
#include "stm32f2xx.h"
#include "BA_board.h"
#include "fileSystem.h"

// #define ENABLE_DBG
#define TRUE 1
#define FALSE 0

// GPS
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

	Delay(2000);
}

char isGPSAlive() {
	uint8_t response[50];
	int i;
	Delay(8000);
	BA_GPSReceive(response, 50);
	for (i = 0; i < 4; i++) {

		BA_GPSSend((uint8_t*) "$PMTK103*37\r\n", 13);	// Full Cold Start
		Delay(2000);
		BA_GPSReceive(response, 19);	// Receive ACK $PMTK001,104,2*32\r\n
#if defined (ENABLE_DBG)
				BA_DBGSend((uint8_t*)response);
#endif
		if ((char*) response[13] == '3') {
#if defined (ENABLE_DBG)
			BA_DBGSend((uint8_t*)" GPS Ack restart OK\r\n");
#endif
			// receive bootup confirmation msgs
			BA_GPSReceive(response, 19);	// Receive $PMTK010,001,2*2E\r\n
#if defined (ENABLE_DBG)
					BA_DBGSend((uint8_t*)response);
#endif

			BA_GPSReceive(response, 19);	// Receive $PMTK011,MTKGPS*08\r\n
			response[19] = '\n';
#if defined (ENABLE_DBG)
			BA_DBGSend((uint8_t*)response);
#endif

			return TRUE;
		} else {
#if defined (ENABLE_DBG)
			BA_DBGSend((uint8_t*)" Reintentando GPS\r\n");
#endif
			// BA_GSMRestart();
		}
	}

	return FALSE;
}

// GPS
void parser_GPS(FIL *fp) {
	uint8_t answer[80];
	uint8_t read[80];

	uint16_t parser_idx = 0;
	uint8_t c;
	int state = 0;	// 0-5 -> searching for $GPGGA header,  1-> getting data

	// Parse for $GPGGA statements
	// $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65\r

	while (1) {
		// BA_DBGSend((uint8_t*)"\r\n New GPGGA reading ... \r\n");
		GPIO_ResetBits(LEDS_GPIO_PORT, LEDR_PIN);
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
		UINT writed;
		f_write(fp, answer, parser_idx, &writed);

		// BA_DBGSendSize(answer,parser_idx);
		// Enciende un led - espera y apaga

		GPIO_SetBits(LEDS_GPIO_PORT, LEDR_PIN);
		break;
		state = 0;
		parser_idx = 0;

		Delay(1000);
	}
	UINT readed;
	f_close(fp);
	f_open(fp,"ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	f_read(fp, &read, parser_idx, &readed);
}

void reset_sector_mkfs_openFile_WriteGPS(){
	FS fileSystem;
	FIL fp;
	reset_sector(0);
	f_mount(&fileSystem,0,0);
	f_mkfs(0);
	f_mount(&fileSystem,0,1);
	f_open(&fp,"ex1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	parser_GPS(&fp);

}

void inicializar() {
	BA_SystimerInit();
	BA_LEDsInit();
	setupGPS();
}

int main(void) {

	inicializar();
	reset_sector_mkfs_openFile_WriteGPS();

	while (1);

	return SUCCESS;
}
