/* 
* ESP.h - ESP header file 
* 
* -----------------------------------------------------------------
*
* Brief description:	Contains ESP functions
*						
* Description:			Contains initialization functions and
*                      interface functions for communication
*                      with ESP WiFi module
*
* -----------------------------------------------------------------
*
* Author: 				Aljaz Prislan
* 
* Date of creation:	14.6.2017
* 
* Date of 1st release: 
* 
* -----------------------------------------------------------------
*
* Revisions:	
*				  
*/
 


#ifndef _ESP_H
#define _ESP_H

//#include <cmsis_os.h>
#include "stm32f10x.h"
//#include "UART.h"
#include "globalData.h"

#define AT_SIZE 					2
#define ATE0_SIZE 				4
#define ATE1_SIZE 				4
#define CWMODE3_SIZE 			11
#define CIPMUX1_SIZE 			17
#define CIPSERVER_SIZE 		17
#define CIPSEND_SIZE 			17
#define CIPCLOSE_SIZE 		13
#define RST_SIZE 					6
#define ATCIFSR_SIZE 			8
#define PACKED_SEND_SIZE 	50

typedef struct {
    const uint8_t AT[AT_SIZE+2];
    const uint8_t ATE0[ATE0_SIZE+2];
		const uint8_t ATE1[ATE1_SIZE+2];
    const uint8_t CWMODE3[CWMODE3_SIZE+2];
    const uint8_t CIPMUX1[CIPMUX1_SIZE+2];
    const uint8_t CIPSERVER[CIPSERVER_SIZE+2];
    uint8_t CIPSEND[CIPSEND_SIZE+2];     // 0-kanal, XYWZ - st. znakov, ki jih bos poslal (odgovor na GET zahtevo)
    uint8_t CIPCLOSE[CIPCLOSE_SIZE+2];
    const uint8_t RST[RST_SIZE+2];
    const uint8_t ATCIFSR[ATCIFSR_SIZE+2];
    
    
    const uint8_t ATsize;
		const uint8_t ATE1size;
    const uint8_t ATE0size;
    const uint8_t CWMODE3size;
    const uint8_t CIPMUX1size;
    const uint8_t CIPSERVERsize;
    const uint8_t CIPSENDsize;
    const uint8_t CIPCLOSEsize;
    const uint8_t RSTsize;
    const uint8_t ATCIFSRsize;
} ATCommandList;

typedef enum ESP_init_states {
	ESP_RESET = 0,
	ESP_ECHO,
	ESP_CWMODE,
	ESP_CIPMUX,
	ESP_CIPSERVER,
	
	ESP_FINISH_INIT
} States;

typedef enum ESP_AT_command {
	AT = 0,
	ATE1,
	ATE0,
	AT_CWMODE3,
	AT_CIPMUX1,
	AT_CIPSERVER,
	AT_CIPSEND,
	AT_CIPCLOSE,
	AT_RESET,
	AT_CIFSR
	
} AT_command_type;

                            
void ESP_init(void);
void ESP_Main_Idle(void);
uint8_t searchReqType(UART_Data *ESPdata);
void ESP_Write(UART_Data *tx, uint8_t iSendType);
void ESP_Read(UART_Data *rx);
void ESP_ReadReq(UART_Data *rx);
uint8_t ESP_ReadATResponse(UART_Data *rx);  // Ali dobimo od ESP response OK, ERROR ali ready
void ESP_HTML_EOT(void);

int send_ATcommand(AT_command_type command);

// for building HTML 
void ESP_HTML_build(UART_Data *ESPdata);
void ESP_HTML_AddToBuffer(UART_Data *ESPdata, char addToPage[]);
void ESP_HTML_setSizeOfCIPSEND(UART_Data *ESPdata);
void ESP_HTML_BuildSenzorTable(UART_Data *ESPdata, uint8_t numOfSenzors);

 
#endif
 