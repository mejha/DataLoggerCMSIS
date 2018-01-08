// file that contains global structs and defines

#ifndef _GLOBAL_DATA_H
#define _GLOBAL_DATA_H

//#include "stm32f10x.h"
#include <cmsis_os.h>
#include <string.h>
#include "UART.h"

// thread
extern osThreadId id_ESP;
extern osThreadId id_UART;
extern osThreadId id_SPI;

extern int iSignalFlag;

// mutex
//extern osMutexId id_mutex_uart;
//osMutexDef(init_mutex_uart);


//strings
#define STR_GET									"GET"
#define STR_POST								"POST"
#define STR_DATE_TIME						"datetime"
#define STR_HTTP								"HTTP"
#define STR_CONNECT							"CONNECT"
#define STR_OK									"OK"
#define STR_READY								"ready"
#define STR_NO_CHANGE						"o change"

// UART defines
#define SIGNAL_READ_DATA		100
#define SIGNAL_SEND_DATA		101
#define SIGNAL_SEND_AT			102
#define UART_READ_RESPONSE	103
#define RECEIVED_REQUEST		104


// ESP
#define FLAG_GET      			1
#define FLAG_POST      			2
#define FLAG_DATE_TIME    	3

#define RESPONSE_OK					4
#define RESPONSE_ERR				5
#define SEND_DATA_OK				6
#define	WAIT_OK_CIPCLOSE		7


#endif
