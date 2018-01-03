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
 
#include "stm32f10x.h"
#include "UART.h"

#ifndef _ESP_H
#define _ESP_H

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

 
//void ESP_reset(void);               // reset ESP module
//void ESP_IP_MAC_set(void);          // set IP and MAC number
//void ESP_init(void);                // initialize ESP module
//void ESP_wp_send(void);             // send web page to ESP (after receiving GET/POST req) 
//void ESP_read(void);                // read message from ESP over UART
                            
void ESP_init(void);
void ESP_check_request(void);
uint8_t searchArrayForRequestType(UART_Data *ESPdata);
void ESP_Write(UART_Data *tx);
void ESP_Read(UART_Data *rx);
void ESP_ReadReq(UART_Data *rx);
uint8_t ESP_ReadATResponse(UART_Data *rx);  // Ali dobimo od ESP response OK, ERROR ali ready
void ESP_error(void);               // nekaj je slo narobe pri komunikaciji z ESP

void T_ESP_WriteKarSiDobil(UART_Data *ESPdata);  // Test funkcija

// for building HTML 
void ESP_HTML_build(UART_Data *ESPdata);
void ESP_HTML_AddToBuffer(UART_Data *ESPdata, char addToPage[]);
void ESP_HTML_setSizeOfCIPSEND(UART_Data *ESPdata);
void ESP_HTML_BuildSenzorTable(UART_Data *ESPdata, uint8_t u8_numOfSenzors);

 
#endif
 