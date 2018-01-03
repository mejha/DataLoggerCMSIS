#include <cmsis_os.h>
#include <stdio.h>

#include "globalData.h"
#include "stm32f10x.h"
#include "RCC.h"
#include "GPIO.h"
#include "SPI.h"
//#include "UART.h"
#include "ESP.h"

#define F_SYSCLK                25000000UL          // Cortex core clock
#define F_APB2CLK               (F_SYSCLK/4)        // GPIO clock

// thread
osThreadId id_ESP;
osThreadId id_UART;
osThreadId id_SPI;

// mutex
osMutexId id_mutex_init;
osMutexDef(init_mutex);
osMutexDef(init_mutex_uart);

void t_main_idle(void const* arg);
void t_UART_idle(void const* arg);
void t_ESP_idle(void const* arg);
void t_SPI_idle(void const* arg);

// thread define for OS
osThreadDef(t_ESP_idle, osPriorityAboveNormal, 1, 0);	// rabimo višjo prioriteto
osThreadDef(t_UART_idle, osPriorityAboveNormal, 1, 0);	// rabimo višjo prioriteto
osThreadDef(t_SPI_idle, osPriorityNormal, 1, 0);


int main()
{
	// naceloma tukaj mutexa ne bi potrebovali, saj threade naredimo sele po izvedeni inicializaciji
	// naredimo ga da sprobamo kako deluje
	id_mutex_init = osMutexCreate(osMutex(init_mutex));  

	// ### wait ### //
	osMutexWait(id_mutex_init, osWaitForever);
	
	// === Initialization ===    
	// Reset and Clock Control
	RCC_init();
	
	GPIO_init();
	//SPI_init();
	UART_init();
	ESP_init();
	
	osMutexRelease(id_mutex_init);
	osDelay(1);
	// ### release ### //
	
	
	//id_SPI = osThreadCreate(osThread(t_SPI_idle), NULL);
	id_UART = osThreadCreate(osThread(t_UART_idle), NULL);
	id_ESP = osThreadCreate(osThread(t_ESP_idle), NULL);
}

void t_ESP_idle(void const* arg)
{
	// ESP idle
	ESP_Main_Idle();
}

void t_UART_idle(void const* arg)
{
	UART_Main_idle();
}

void t_SPI_idle(void const* arg)
{
	// idi v neki idle
}
