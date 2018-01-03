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
#define SIGNAL_READ_DATA		1
#define SIGNAL_SEND_DATA		2
#define SIGNAL_SEND_AT			3
#define UART_READ_RESPONSE	4


// ESP
#define FLAG_GET      			1
#define FLAG_POST      			2
#define FLAG_DATE_TIME    	3


#endif
