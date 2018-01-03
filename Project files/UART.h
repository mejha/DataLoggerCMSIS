/* 
 * UART.h - UART driver header file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains UART function prototypes
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	4.6.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	
 *				  
 */
 


#ifndef _UART_H
#define _UART_H

#include "stm32f10x.h"
//#include <cmsis_os.h>
#include "globalData.h"

#define UART_TX_RX_BUFFER_LEN   2000                // buffer dolzina za rx in tx transmissions
#define EOT                     (uint8_t)4          // ASCII End Of Transmission
#define ETB                     (uint8_t)23         // ASCII End of Transmission Block

typedef struct {
    uint8_t parityError;
    uint8_t breakCharDetected;
    uint8_t EOTChar;
    uint8_t bufferLenExceeded;
    uint8_t ESPTxEnd;
} UART_Status;

typedef struct {
	uint8_t buffer[UART_TX_RX_BUFFER_LEN];
	const uint8_t *pBuffer;
	uint32_t lengthAT;  	// dolzina za AT komande        // st znakov, ki jih zelimo poslati oz. stevilo znakov, ki jih zelimo prebrati
	uint32_t lengthUART; 	// dolzina za string (HTML,...)
	UART_Status rxStatus;
	
	// channel send data to (+IPD)
	uint8_t iChannelSendNum;
} UART_Data;

extern UART_Data ESPdata;


void UART_Main_idle(void);

void UART_init(void);
void UART_Write(UART_Data *tx);
void UART_WriteP(UART_Data *tx);
void UART_Read(UART_Data *rx);
void UART_ReadReq(UART_Data *rx);
void resetRxStatus(UART_Status*, uint8_t breakChar, uint8_t bufferLen, uint8_t EOTChar, uint8_t parity, uint8_t ESPTxEnd);

#endif
