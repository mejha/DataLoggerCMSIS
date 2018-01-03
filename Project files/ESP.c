/* 
 * ESP.c - ESP driver file 
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
 
#include <stdio.h>
#include <cmsis_os.h>

#include "stm32f10x.h"
#include "globalData.h"
#include "ESP.h"
//#include "UART.h"
#include "GPIO.h"

// mutex
osMutexId id_mutex_ESP_UART;

static ATCommandList ATcommand = {  { 'A', 'T', '\r', '\n' },                                                                               // .AT        = { 'A', 'T', '\n', 0xD },
                                    { 'A', 'T', 'E', '0', '\r', '\n' },                                                                     // .ATE0
																		{ 'A', 'T', 'E', '1', '\r', '\n' },																																			// .ATE1
                                    { 'A', 'T', '+', 'C', 'W', 'M', 'O', 'D', 'E', '=', '3', '\r', '\n'  },                                 // .CWMODE3   = { 'A', 'T', '+', 'C', 'W', 'M', 'O', 'D', 'E', '=', '3', '\n', 0xD  },
                                    { 'A', 'T', '+', 'C', 'I', 'P', 'M', 'U', 'X', '=', '1', '\r', '\n'  },                                 // .CIPMUX1   = { 'A', 'T', '+', 'C', 'I', 'P', 'M', 'U', 'X', '=', '1', '\n', 0xD  },
                                    { 'A', 'T', '+', 'C', 'I', 'P', 'S', 'E', 'R', 'V', 'E', 'R', '=', '1', ',', '8', '0', '\r', '\n'  },   // .CIPSERVER = { 'A', 'T', '+', 'C', 'I', 'P', 'S', 'E', 'R', 'V', 'E', 'R', '=', '1', ',', '8', '0', '\n', 0xD  },
                                    { 'A', 'T', '+', 'C', 'I', 'P', 'S', 'E', 'N', 'D', '=', '0', ',', '0', '0', '0', '5', '\r', '\n'  },   // .CIPSEND   = { 'A', 'T', '+', 'C', 'I', 'P', 'S', 'E', 'N', 'D', '=', '0', ',', 'X', 'Y', 'W', 'Z', '\n', 0xD  },     // 0-kanal, XYWZ - st. znakov, ki jih bos poslal (odgovor na GET zahtevo)
                                    { 'A', 'T', '+', 'C', 'I', 'P', 'C', 'L', 'O', 'S', 'E', '=', '0', '\r', '\n'  },                       // .CIPCLOSE  = { 'A', 'T', '+', 'C', 'I', 'P', 'C', 'L', 'O', 'S', 'E', '=', '0', '\n', 0xD  },
                                    { 'A', 'T', '+', 'R', 'S', 'T', '\r', '\n'  },                                                          // .RST       = { 'A', 'T', '+', 'R', 'S', 'T', '\n', 0xD  },
                                    { 'A', 'T', '+', 'C', 'I', 'F', 'S', 'R', '\r', '\n'  },                                                // .ATCIFSR   = { 'A', 'T', '+', 'C', 'I', 'F', 'S', 'R', '\n', 0xD  },
                             
                                    AT_SIZE+2,         // .ATsize        = AT_SIZE+2,
																		ATE0_SIZE+2,
                                    ATE1_SIZE+2,       // .ATE0size      = ATE0_SIZE+2
                                    CWMODE3_SIZE+2,    // .CWMODE3size   = CWMODE3_SIZE+2,
                                    CIPMUX1_SIZE+2,    // .CIPMUX1size   = CIPMUX1_SIZE+2,
                                    CIPSERVER_SIZE+2,  // .CIPSERVERsize = CIPSERVER_SIZE+2,
                                    CIPSEND_SIZE+2,    // .CIPSENDsize   = CIPSEND_SIZE+2,
                                    CIPCLOSE_SIZE+2,   // .CIPCLOSEsize  = CIPCLOSE_SIZE+2,
                                    RST_SIZE+2,        // .RSTsize       = RST_SIZE+2,
                                    ATCIFSR_SIZE+2     // .ATCIFSRsize   = ATCIFSR_SIZE+2 
                                };

// ESP HTML fixed strings
#define HTML_INIT "<!DOCTYPE html>"
#define HTML_META_REFRESH "<meta http-equiv=\"refresh\" content=\"15\">"
#define HTML_STYLE "<style>table,th,td{border: 1px solid black;border-collapse: collapse;}th,td{padding: 5px;text-align: left;}</style>"
#define HTML_FORM_DATE_TIME "<form><button type=\"submit\" formmethod=\"post\">Set time</button>Time (dd/mm/yyyy hh/mm): <input type=\"text\" name=\"datetime\"></form>"
																
#define HTML_TABLE_STYLE "<table style=\"width:60%\">"
#define HTML_START_TR	"<tr>"
#define HTML_START_TH "<th>"
#define HTML_START_TD "<td>"

#define HTML_COLSPAN "<th colspan=\"2\">"

#define HTML_END_TR "</tr>"
#define HTML_END_TH "</th>"
#define HTML_END_TD "</td>"
#define HTML_END_TABLE "</table>"
																
// ESP response flag															
#define RECIEVED_GET 		1
#define RECIEVED_POST		2	

#define UART_SEND_TYPE_AT			1
#define UART_SEND_TYPE_UART		2
	
																
int send_ATcommand(AT_command_type command)
{
	
	switch(command)
	{
		case AT:
			ESPdata.pBuffer = ATcommand.AT;
			ESPdata.lengthAT  = ATcommand.ATsize;
		break;
		
		case ATE1:
			ESPdata.pBuffer = ATcommand.ATE1;
			ESPdata.lengthAT  = ATcommand.ATE1size;
		break;
		
		case ATE0:
			ESPdata.pBuffer = ATcommand.ATE0;
			ESPdata.lengthAT  = ATcommand.ATE0size;
		break;
		
		case AT_CWMODE3:
			ESPdata.pBuffer = ATcommand.CWMODE3;
			ESPdata.lengthAT = ATcommand.CWMODE3size;
		break;
		
		case AT_CIPMUX1:
			ESPdata.pBuffer = ATcommand.CIPMUX1;
			ESPdata.lengthAT = ATcommand.CIPMUX1size;
		break;
		
		case AT_CIPSERVER:
			ESPdata.pBuffer = ATcommand.CIPSERVER;
			ESPdata.lengthAT = ATcommand.CIPSERVERsize;
		break;
		
		case AT_CIPSEND:
			ATcommand.CIPSEND[11] = ESPdata.iChannelSendNum + '0';		
			ESP_HTML_setSizeOfCIPSEND(&ESPdata);
			ESPdata.pBuffer = ATcommand.CIPSEND;					
			ESPdata.lengthAT = ATcommand.CIPSENDsize;
		break;
		
		case AT_CIPCLOSE:
			ATcommand.CIPCLOSE[12] = ESPdata.iChannelSendNum + '0';
			ESPdata.pBuffer = ATcommand.CIPCLOSE;
			ESPdata.lengthAT = ATcommand.CIPCLOSEsize;
		break;
		
		case AT_RESET:
			ESPdata.pBuffer = ATcommand.RST;
			ESPdata.lengthAT = ATcommand.RSTsize;
		break;
		
//		case AT_CIFSR:
//		break;
		
		default:
		return 1; // error
	}
	
	//ESP_Write(&ESPdata, UART_SEND_TYPE_AT);
	
		osSignalSet(id_UART, SIGNAL_SEND_AT);
		osDelay(1);
		osSignalWait(0, osWaitForever);
		osDelay(1);
	
	return 0; // ok
}
	
// ESP inicializacija
void ESP_init()
{
  uint32_t cnt = 0;
	uint8_t ESP_state = ESP_RESET;
	int iStateCounterSafety = 0;		// zascita, ce bi se ESP slucajno cikal -> ga resetiraj
    
	while (ESP_state != ESP_FINISH_INIT)
	{
		for(cnt=0; cnt<1000000; ++cnt);
		
		if(iStateCounterSafety > 5)
		{
			ESP_state = ESP_RESET;
			iStateCounterSafety = 0;
		}
		
		switch(ESP_state) 
		{	
			case ESP_RESET:
				//send_ATcommand(AT_RESET);
			ESPdata.pBuffer = ATcommand.RST;
			ESPdata.lengthAT = ATcommand.RSTsize;
			UART_WriteP(&ESPdata);
			
			ESP_state = ESP_ECHO;
			for(cnt=0; cnt<1000000; ++cnt);
			break;
			
			case ESP_ECHO:				// echo off
				//send_ATcommand(ATE0);
			ESPdata.pBuffer = ATcommand.ATE0;
			ESPdata.lengthAT  = ATcommand.ATE0size;
			UART_WriteP(&ESPdata);
				
			if(ESP_ReadATResponse(&ESPdata))
				ESP_state = ESP_CWMODE;
			else
				iStateCounterSafety++;				
			break;
			
			case ESP_CWMODE: 			// set ESP in AP and client mode	
				//send_ATcommand(AT_CWMODE3);
			ESPdata.pBuffer = ATcommand.CWMODE3;
			ESPdata.lengthAT  = ATcommand.CWMODE3size;
			UART_WriteP(&ESPdata);

			if(ESP_ReadATResponse(&ESPdata))
				ESP_state = ESP_CIPMUX;
			else
				iStateCounterSafety++;
			break;
			
			case ESP_CIPMUX: 			// allow multiple connections
				//send_ATcommand(AT_CIPMUX1);
			ESPdata.pBuffer = ATcommand.CIPMUX1;
			ESPdata.lengthAT  = ATcommand.CIPMUX1size;
			UART_WriteP(&ESPdata);
			
			if(ESP_ReadATResponse(&ESPdata))
				ESP_state = ESP_CIPSERVER;
			else
				iStateCounterSafety++;
			break;
			
			case ESP_CIPSERVER:	  // server mode & port 80 set
				//send_ATcommand(AT_CIPSERVER);
			ESPdata.pBuffer = ATcommand.CIPSERVER;
			ESPdata.lengthAT  = ATcommand.CIPSERVERsize;
			UART_WriteP(&ESPdata);
			
			if(ESP_ReadATResponse(&ESPdata))
				ESP_state = ESP_FINISH_INIT;
			else
				iStateCounterSafety++;
			break;
			
			default:
			ESP_state = ESP_RESET;
			iStateCounterSafety = 0;
			break;
		}
	}
}

void ESP_HTML_EOT(void)
{
	ESPdata.buffer[ESPdata.lengthUART++] = 0x0D;
	ESPdata.buffer[ESPdata.lengthUART++] = 0x0A;
}

// add elements to buffer
void ESP_HTML_AddToBuffer(UART_Data *ESPdata, char addToPage[])
{
	int counter = 0;
	int sizeOfChar = 0;
	
//	for(counter=0; addToPage[counter]; counter++) // izracuna dolzino char
//		sizeOfChar ++;
	
	sizeOfChar = strlen(addToPage);

	for (counter = 0; counter < sizeOfChar; counter++)
		ESPdata->buffer[ESPdata->lengthUART + counter] = addToPage[counter];
	
	ESPdata->lengthUART = ESPdata->lengthUART + sizeOfChar;
}

// Zgradi tabelo z u8_numOfSenzors stevilom senzorjem
void ESP_HTML_BuildSenzorTable(UART_Data *ESPdata, uint8_t numOfSenzors)
{
	uint8_t numOfRows = 5;
	int i,j;
	char cSenzorBuff[] = "Senzor 1 ";
	
	ESP_HTML_AddToBuffer(ESPdata, HTML_TABLE_STYLE);
	ESP_HTML_AddToBuffer(ESPdata, HTML_START_TR);
	
	for(i = 0; i < numOfSenzors; i++) // dodamo imena senzorjev
	{
		cSenzorBuff[7] = i + 1 + '0';

		ESP_HTML_AddToBuffer(ESPdata, HTML_COLSPAN); // vrednosti senzorjev bo potrebno prebrati iz SD kartice
		ESP_HTML_AddToBuffer(ESPdata, cSenzorBuff);
		ESP_HTML_AddToBuffer(ESPdata, "[°C]");
		ESP_HTML_AddToBuffer(ESPdata, HTML_END_TH);
	}
	
	ESP_HTML_AddToBuffer(ESPdata, HTML_END_TR);
	
	for(i = 0; i < numOfRows; i++) // stevilo vrstic
	{
		ESP_HTML_AddToBuffer(ESPdata, HTML_START_TR);
		
		for(j = 0; j < numOfSenzors; j++) // dodamo vrednosti senzorjev, zaenkrat imamo testne vrednosti, funkcijo je potrebno dodelati
		{
			ESP_HTML_AddToBuffer(ESPdata, HTML_START_TD);
			ESP_HTML_AddToBuffer(ESPdata, "12");
			ESP_HTML_AddToBuffer(ESPdata, HTML_END_TD);
			
			ESP_HTML_AddToBuffer(ESPdata, HTML_START_TD);
			ESP_HTML_AddToBuffer(ESPdata, "10/1/2017 10:10");
			ESP_HTML_AddToBuffer(ESPdata, HTML_END_TD);
		}
		
		ESP_HTML_AddToBuffer(ESPdata, HTML_END_TR);
	}

	ESP_HTML_AddToBuffer(ESPdata, HTML_END_TABLE);
}

// here we create HTML page
void ESP_HTML_build(UART_Data *ESPdata) 
{
	uint8_t senzorNum = 4;
	
	ESPdata->lengthUART = 0;
	//start HMTL
	ESP_HTML_AddToBuffer(ESPdata, HTML_INIT);
	ESP_HTML_AddToBuffer(ESPdata, "<hmtl>");
	
	// head
	ESP_HTML_AddToBuffer(ESPdata, "<head>");
	ESP_HTML_AddToBuffer(ESPdata, HTML_META_REFRESH);
	ESP_HTML_AddToBuffer(ESPdata, HTML_STYLE);
	ESP_HTML_AddToBuffer(ESPdata, "</head>");
	
	// table
	ESP_HTML_AddToBuffer(ESPdata, "<body>");
	ESP_HTML_BuildSenzorTable(ESPdata, senzorNum);
	
	// time
	ESP_HTML_AddToBuffer(ESPdata, "<p>Current Time: 16:10</p>");
	
	// form
	ESP_HTML_AddToBuffer(ESPdata, HTML_FORM_DATE_TIME);
	
	ESP_HTML_AddToBuffer(ESPdata, "</body>");
	ESP_HTML_AddToBuffer(ESPdata, "</html>");
}

void ESP_HTML_setSizeOfCIPSEND(UART_Data *ESPdata)
{
	if (ESPdata->lengthUART > 999)
	{
		ATcommand.CIPSEND[13] = (ESPdata->lengthUART / 1000) % 10 + '0';
		ATcommand.CIPSEND[14] = (ESPdata->lengthUART / 100) % 10 + '0';
		ATcommand.CIPSEND[15] = (ESPdata->lengthUART / 10) % 10 + '0';
		ATcommand.CIPSEND[16] =  ESPdata->lengthUART % 10 + '0';
	}
	else if ((ESPdata->lengthUART > 99) && (ESPdata->lengthUART < 1000))
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = (ESPdata->lengthUART / 100) % 10 + '0';
		ATcommand.CIPSEND[15] = (ESPdata->lengthUART / 10) % 10 + '0';
		ATcommand.CIPSEND[16] =  ESPdata->lengthUART % 10 + '0';
	}
	else if ((ESPdata->lengthUART > 9) && (ESPdata->lengthUART < 100))
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = '0';
		ATcommand.CIPSEND[15] = ESPdata->lengthUART / 10 + '0';
		ATcommand.CIPSEND[16] = ESPdata->lengthUART % 10 + '0';
	}
	else
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = '0';
		ATcommand.CIPSEND[15] = '0';
		ATcommand.CIPSEND[16] = ESPdata->lengthUART + '0';
	}
}


void ESP_Main_Idle()
{
	uint8_t requestType = 0;
	
	while(1)
	{
		osSignalSet(id_UART, SIGNAL_READ_DATA);
		osDelay(1);
		
		osSignalWait(0, osWaitForever);
		osDelay(1);
		
		requestType = searchReqType(&ESPdata);
		
		switch(requestType) 
		{
			case FLAG_GET:
			ESPdata.lengthUART = 0;
			ESP_HTML_build(&ESPdata);	// zgradimo stran

			send_ATcommand(AT_CIPSEND);
			osDelay(20);
			osSignalSet(id_UART, SIGNAL_SEND_DATA);	// send html page
			osDelay(1);
			
			osSignalWait(0, 500);
			osDelay(1);				
			
			send_ATcommand(AT_CIPCLOSE);
			break;
			
			case FLAG_POST:     // POST
					//dealWithPOST();
					break;
			default:
					break;
		}
	}
}

uint8_t searchReqType(UART_Data *ESPdata)
{
	int i, iFlag = 0;
	
	if(strstr((const char*) &ESPdata->buffer[0], STR_GET))
		iFlag = FLAG_GET;
	
	if(strstr((const char*) &ESPdata->buffer[0], STR_POST) && (strstr((const char*) &ESPdata->buffer[0], STR_DATE_TIME)))
		iFlag = FLAG_POST;
	
	for(i = 0; i < (UART_TX_RX_BUFFER_LEN-2); ++i)
	{
		if((ESPdata->buffer[i] == ',') && (ESPdata->buffer[i + 2] == ','))
		{
			ESPdata->iChannelSendNum = ESPdata->buffer[i + 1] - '0';
			break;
		}
	}
	
	return iFlag;
}

void ESP_Write(UART_Data *tx, uint8_t iSendType)
{
	if(iSendType == UART_SEND_TYPE_AT)
	{
		osSignalSet(id_UART, SIGNAL_SEND_AT);
    UART_WriteP(tx);
	}
	
	else if(iSendType == UART_SEND_TYPE_UART)
	{
		osSignalSet(id_UART, SIGNAL_SEND_DATA);
    UART_Write(tx);
	}
}


// beri response od AT komande
void ESP_Read(UART_Data *rx)
{
    UART_Read(rx);
}

// beri request (GET, POST)
void ESP_ReadReq(UART_Data *rx)
{
    UART_ReadReq(rx);
}

// beri odgovor na AT komando, preveri ce je odgovor vredu ("OK" ali "ready")
uint8_t ESP_ReadATResponse(UART_Data *rx)
{
    UART_Read(rx);
    
    if(strstr((const char*) &rx->buffer[0], STR_OK))
    { // response is 'OK' aka ok
        return 1;
    }
    else if(strstr((const char*) &rx->buffer[0], STR_READY))
    { // response is 'ready' aka ok
        return 1;
    }
		else if(strstr((const char*) &rx->buffer[0], STR_NO_CHANGE))
		{	// no change ->ok
			return 1;
		}
    
    return 0;
}
 
