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

#include "stm32f10x.h"
#include "ESP.h"
#include "UART.h"
#include "GPIO.h"

static UART_Data ESPdata;

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

								
// novi poiskus implementacije
#define HTML_INIT "<!DOCTYPE html>"
#define HTML_DATE_TIME "<form><button type=\"submit\" formmethod=\"post\">Set time</button>Time (dd/mm/yyyy hh/mm): <input type=\"text\" name=\"datetime\"></form>"
#define HTML_CURRENT_TIME "Current Time: "
#define HTML_META_REFRESH "<meta http-equiv=\"refresh\" content=\"15\">"
#define HTML_STYLE "<style>table,th,td{border: 1px solid black;border-collapse: collapse;}th,td{padding: 5px;text-align: left;}</style>"
#define HTML_FORM_DATE_TIME "<form><button type=\"submit\" formmethod=\"post\">Set time</button>Time (dd/mm/yyyy hh/mm): <input type=\"text\" name=\"datetime\"></form>"
	
enum ESP_init_states {
	ESP_RESET = 0,
	ESP_ECHO,
	ESP_CWMODE,
	ESP_CIPMUX,
	ESP_CIPSERVER,
	
	ESP_STATE_NUM,
	ESP_FINISH_INIT
};
	
// ESP inicializacija
void ESP_init()
{
    uint32_t cnt = 0;
	uint8_t ESP_state = ESP_ECHO;
    
	while (ESP_state < ESP_STATE_NUM) 
	{
		for(cnt=0; cnt<1000000; ++cnt);
		
		switch(ESP_state) {
			
			case ESP_RESET:
				ESPdata.pBuffer = ATcommand.RST;
				ESPdata.length = ATcommand.RSTsize;
				ESP_Write(&ESPdata);
			
				if(ESP_ReadATResponse(&ESPdata))
					ESP_state = ESP_ECHO;
			break;
			
			case ESP_ECHO:				
				ESPdata.pBuffer = ATcommand.ATE0;
				ESPdata.length  = ATcommand.ATE0size;
				ESP_Write(&ESPdata);
				
				if(ESP_ReadATResponse(&ESPdata))
					ESP_state = ESP_CWMODE;				
			break;
			
			case ESP_CWMODE: 				
				// set ESP in AP and client mode
				ESPdata.pBuffer = ATcommand.CWMODE3;
				ESPdata.length = ATcommand.CWMODE3size;
				ESP_Write(&ESPdata);

				if(ESP_ReadATResponse(&ESPdata))
					ESP_state = ESP_CIPMUX;							
			break;
			
			case ESP_CIPMUX: 				
				// allow multiple connections
				ESPdata.pBuffer = ATcommand.CIPMUX1;
				ESPdata.length = ATcommand.CIPMUX1size;
				ESP_Write(&ESPdata);
			
				if(ESP_ReadATResponse(&ESPdata))
					ESP_state = ESP_CIPSERVER;		
			break;
			
			case ESP_CIPSERVER: 
				// server mode & port 80 set
				ESPdata.pBuffer = ATcommand.CIPSERVER;
				ESPdata.length = ATcommand.CIPSERVERsize;
				ESP_Write(&ESPdata);
			
				if(ESP_ReadATResponse(&ESPdata))
					ESP_state = ESP_FINISH_INIT;		
			break;
			
			default:
				ESP_state = ESP_RESET;
			break;
		}
	}
}

// add elements to buffer
void ESP_HTML_AddToBuffer(UART_Data *ESPdata, char addToPage[])
{
	uint8_t counter = 0;
	uint8_t sizeOfChar = 0;
	
	for(counter=0; addToPage[counter]; counter++) // izracuna dolzino char
		sizeOfChar ++;

	for (counter = 0; counter < sizeOfChar; counter++)
		ESPdata->buffer[ESPdata->length + counter] = addToPage[counter];
	
	ESPdata->length = ESPdata->length + sizeOfChar;
}

// Zgradi tabelo z u8_numOfSenzors stevilom senzorjem
void ESP_HTML_BuildSenzorTable(UART_Data *ESPdata, uint8_t u8_numOfSenzors)
{
	uint8_t u8_numOfRows = 5;
	int i,j;
	char cSenzorBuff[] = "Senzor 1 ";
	
	ESP_HTML_AddToBuffer(ESPdata, "<table style=\"width:60%\">");
	ESP_HTML_AddToBuffer(ESPdata, "<tr>");
	
	for(i = 0; i < u8_numOfSenzors; i++) // dodamo imena senzorjev
	{
		cSenzorBuff[7] = i + 1 + '0';

		ESP_HTML_AddToBuffer(ESPdata, "<th colspan=\"2\">"); // vrednosti senzorjev bo potrebno prebrati iz SD kartice
		ESP_HTML_AddToBuffer(ESPdata, cSenzorBuff);
		ESP_HTML_AddToBuffer(ESPdata, "[°C]</th>");
	}
	
	ESP_HTML_AddToBuffer(ESPdata, "</tr>");
	
	for(i = 0; i < u8_numOfRows; i++) // stevilo vrstic
	{
		ESP_HTML_AddToBuffer(ESPdata, "<tr>");
		
		for(j = 0; j < u8_numOfSenzors; j++) // dodamo vrednosti senzorjev, zaenkrat imamo testne vrednosti, funkcijo je potrebno dodelati
		{
			ESP_HTML_AddToBuffer(ESPdata, "<td>");
			ESP_HTML_AddToBuffer(ESPdata, "12");
			ESP_HTML_AddToBuffer(ESPdata, "</td>");
			
			ESP_HTML_AddToBuffer(ESPdata, "<td>");
			ESP_HTML_AddToBuffer(ESPdata, "10/1/2017 10:10");
			ESP_HTML_AddToBuffer(ESPdata, "</td>");
		}
		
		ESP_HTML_AddToBuffer(ESPdata, "</tr>");
	}

	ESP_HTML_AddToBuffer(ESPdata, "</table>");
}

// here we create HTML page
void ESP_HTML_build(UART_Data *ESPdata) 
{
	uint8_t senzorNum = 4;
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
	if (ESPdata->length > 999)
	{
		ATcommand.CIPSEND[13] = (ESPdata->length / 1000) % 10 + '0';
		ATcommand.CIPSEND[14] = (ESPdata->length / 100) % 10 + '0';
		ATcommand.CIPSEND[15] = (ESPdata->length / 10) % 10 + '0';
		ATcommand.CIPSEND[16] = ESPdata->length % 10 + '0';
	}
	else if ((ESPdata->length > 99) && (ESPdata->length < 1000))
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = (ESPdata->length / 100) % 10 + '0';
		ATcommand.CIPSEND[15] = (ESPdata->length / 10) % 10 + '0';
		ATcommand.CIPSEND[16] = ESPdata->length % 10 + '0';
	}
	else if ((ESPdata->length > 9) && (ESPdata->length < 100))
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = '0';
		ATcommand.CIPSEND[15] = ESPdata->length / 10 + '0';
		ATcommand.CIPSEND[16] = ESPdata->length % 10 + '0';
	}
	else
	{
		ATcommand.CIPSEND[13] = '0';
		ATcommand.CIPSEND[14] = '0';
		ATcommand.CIPSEND[15] = '0';
		ATcommand.CIPSEND[16] = ESPdata->length + '0';
	}
}


void ESP_check_request()
{
    uint8_t requestType = 0;
    uint32_t cnt = 0;
	uint32_t tempLenght = 0;
    
    ESP_ReadReq(&ESPdata);
    
    requestType = searchArrayForRequestType(&ESPdata);
    
    switch(requestType) {
        case 1:     // GET						
			ESPdata.length = 0;
			ESP_HTML_build(&ESPdata);
	
			tempLenght = ESPdata.length; // zacasno shranimo dolzino
			
			// nastavimo kanal, poiscemo ga v searchArrayForRequestType
			ATcommand.CIPSEND[11] = ESPdata.iChannelNum + '0';		
			ESP_HTML_setSizeOfCIPSEND(&ESPdata);
			ESPdata.pBuffer = ATcommand.CIPSEND;					
			ESPdata.length = ATcommand.CIPSENDsize;
			ESP_Write(&ESPdata);	// ESP_Write se uporablja za posiljanje komand, UART_Write pa za posiljanje iz bufferja

			ESPdata.length = tempLenght;	
		
			for(cnt=0; cnt<400000; ++cnt);	// nevem zakaj mora biti tuki delay!!!! TODO, weird shit - slabo, upocasni posiljanje podatkov proti esp-ju grrrr
		
			UART_Write(&ESPdata);

			for(cnt=0; cnt<400000; ++cnt);	// nevem zakaj mora biti tuki delay!!!! TODO, mogoce ESP ni dovolj hiter?
	
			ATcommand.CIPCLOSE[12] = ESPdata.iChannelNum + '0';
			ESPdata.pBuffer = ATcommand.CIPCLOSE;
            ESPdata.length = ATcommand.CIPCLOSEsize;
            ESP_Write(&ESPdata);

            break;
        case 2:     // POST
            //dealWithPOST();
            break;
        default:
            break;
    }
}

uint8_t searchArrayForRequestType(UART_Data *ESPdata)
{
    // zastavice
    uint8_t foundPOST = 0;
    uint8_t foundDatetime = 0;
    // dolzine
    const uint8_t vsebinaGETLen = 4;
    const uint8_t vsebinaPOSTLen = 5;
    const uint8_t vsebinaDatetimeLen = 9;
    // vsebina, ki jo iscemo
    uint8_t vsebinaGET[vsebinaGETLen] = "GET";
    uint8_t vsebinaPOST[vsebinaPOSTLen] = "POST";
    uint8_t vsebinaDatetime[vsebinaDatetimeLen] = "datetime";

    // pozicija
    uint32_t pos_buffer=0;
    uint8_t pos_vsebina = 0;
 
    uint8_t dan = 0;
    uint8_t mesec = 0;
    uint16_t leto = 0;
    uint8_t ura = 0;
    uint8_t minute = 0;
     
    
    // search for GET
    for(pos_buffer=0; pos_buffer<(UART_TX_RX_BUFFER_LEN-2); ++pos_buffer)       // -1 zaradi indeksov, -1 zaradi \n
    {
        if(ESPdata->buffer[pos_buffer] == vsebinaGET[pos_vsebina])
        {
            ++pos_vsebina;
            if(pos_vsebina >= vsebinaGETLen-1)      // >=3
            {
                return 1;
            }
        }
        else
        {
            pos_buffer = pos_buffer - pos_vsebina;
            pos_vsebina=0;
        }
				
		if((ESPdata->buffer[pos_buffer] == ',') && 
			 (ESPdata->buffer[pos_buffer + 2] == ',')) 	// za iskanje vejice, poiskati je potrebno 2 vejici
		{																														// predvidevamo da lahko imamo najvec 10 kanalov (enomestno stevilo) -> 0-9
				ESPdata->iChannelNum = ESPdata->buffer[pos_buffer + 1] - '0';
		}			
    }
    
    // search for POST
    pos_buffer=0;
    pos_vsebina = 0;
    
    for(pos_buffer=0; pos_buffer<(UART_TX_RX_BUFFER_LEN-2); ++pos_buffer)
    {
        if(ESPdata->buffer[pos_buffer] == vsebinaPOST[pos_vsebina])
        {
            ++pos_vsebina;
            if(pos_vsebina >= vsebinaGETLen-1)      // >=4
            {
                foundPOST = 1;
                break;
            }
        }
        else
        {
            pos_buffer = pos_buffer - pos_vsebina;
            pos_vsebina=0;
        }
    }
    
    // if POST, poisci ali gre za nast. casa ali podatek iz senzorja
    if(foundPOST)
    {
        pos_buffer = ESPdata->length - 1;     // pojdi na zadnjo mesto
        pos_vsebina = 0;
        
        if(pos_buffer >= 2) // protekcija
        {
            // isci od zadaj naprej
            for(pos_vsebina=0; pos_vsebina<pos_buffer-2; ++pos_vsebina)
            {
                if((ESPdata->buffer[pos_buffer-pos_vsebina-1] == '\n') &&     // posici lokacijo, kjer se nahajata dva '\n'
                   (ESPdata->buffer[pos_buffer-pos_vsebina-2] == '\n'))
                {
                    pos_buffer = pos_buffer-pos_vsebina;
                    break;
                }
            }
            
            foundDatetime = 1;
            
            for(pos_vsebina=0; pos_vsebina<vsebinaDatetimeLen-1; ++pos_vsebina)     // -1 zaradi '\n'
            {
                if(ESPdata->buffer[pos_buffer+pos_vsebina] != vsebinaDatetime[pos_vsebina])     // ali se na tem mestu nahaja "datetime=..." ?
                {
                    foundDatetime = 0;
                }
            }
            
            if(foundDatetime == 1)
            {
                pos_buffer += vsebinaDatetimeLen;
                
                dan = 10*(ESPdata->buffer[pos_buffer] - '0');
                dan +=    ESPdata->buffer[pos_buffer+1] - '0';
                
                mesec = 10*(ESPdata->buffer[pos_buffer+3] - '0');
                mesec +=    ESPdata->buffer[pos_buffer+4] - '0';
                
                leto = 1000*(ESPdata->buffer[pos_buffer+6] - '0');
                leto += 100*(ESPdata->buffer[pos_buffer+7] - '0');
                leto +=  10*(ESPdata->buffer[pos_buffer+8] - '0');
                leto +=     (ESPdata->buffer[pos_buffer+9] - '0');
                
                ura = 10*(ESPdata->buffer[pos_buffer+11] - '0');
                ura +=    ESPdata->buffer[pos_buffer+12] - '0';
                
                minute = 10*(ESPdata->buffer[pos_buffer+14] - '0');
                minute +=    ESPdata->buffer[pos_buffer+15] - '0';
                
                return 2;
            }
        }
    }
    
    return 0;
}


void ESP_Write(UART_Data *tx)
{
    UART_WriteP(tx);
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
    
    if( (rx->buffer[0] == 'O') &&
        (rx->buffer[1] == 'K'))
    {
        // response is 'OK' aka ok
        return 1;
    }
    else if((rx->buffer[0] == 'r') &&
            (rx->buffer[1] == 'e'))
    {
        // response is 'ready' aka ok
        return 1;
    }
    
    return 0;
}


// Test funkcija
// Kar si prebral kot reposnes na AT komando (hex stevilka), pretvori v ASCII in poslji
void T_ESP_WriteKarSiDobil(UART_Data *data)
{
    UART_Write(data);
}


// ESP error
void ESP_error()
{
    GPIOB->ODR |= GPIO_ODR_ODR4;
    while(1)
    {
    }
}
 
