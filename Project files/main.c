#include <cmsis_os.h>
#include <stdio.h>

#include "globalData.h"
#include "stm32f10x.h"
#include "RCC.h"
#include "GPIO.h"
#include "SPI.h"
//#include "UART.h"
#include "ESP.h"
#include "SDcard.h"

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

// test zadeve
// ===========
#define SD_TEST
void test_funkcija_za_prislankota(int kerJeMihaNekiZajebal_sadface);
// ===========

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
	
	//test_funkcija_za_prislankota(42);	// 42 answer to life
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

void test_funkcija_za_prislankota(int kerJeMihaNekiZajebal_sadface)
{
		static UART_Data rxData, txData;
	  uint8_t shift_count = 0;
    volatile uint8_t flag = 0;
    uint8_t packet1 = 0xAA;
    uint8_t packet2 = 0xBB;
    uint8_t one = '1';
    uint8_t jst[] = { 'A', 'l', 'j', 'a', 'z' };
    uint8_t receivedPacket = 0;
    uint32_t i = 0;
    
    
    uint8_t tekst_init[17] = "SD card init...\n";
    uint8_t tekst_complete[10] = "complete\n";
    uint8_t tekst_fail[8] = "failed\n";
    uint8_t tekst_read[29] = "SD card read in progress...\n";
    uint8_t tekst_write[30] = "SD card write in progress...\n";
    
    uint8_t tekst_blok[33] = ". byte v bloku prebran pravilno: ";
    uint8_t tekst_da[3] = { 'd', 'a', '\n' };
    uint8_t tekst_ne[3] = { 'n', 'e', '\n' };
    uint8_t tekst_temp;
    
    uint8_t blokpodatkov[512];
    uint8_t blokbuffer[512] = { 0 };
	
	#ifdef SPI_TEST                    
    while(1)
    {
        //GPIOD->ODR ^= 0xffff;
        
        for(;i<10000;++i);
        i = 0;
        
        
        
        if(((GPIOE->IDR & GPIO_IDR_IDR15) == GPIO_IDR_IDR15) && (flag == 0))
        {
            SPI_TransmitReceive(&packet1, &receivedPacket, 1);
            SPI_TransmitReceive(&packet2, &receivedPacket, 1);
            GPIOD->ODR ^= GPIO_ODR_ODR3;
            flag = 1;
        }
        
        if(((GPIOE->IDR & GPIO_IDR_IDR11) == GPIO_IDR_IDR11))
        {
            receivedPacket = 0;
            flag = 0;
        }
        
        if((receivedPacket == packet1) || (receivedPacket == packet2))
        {
            GPIOD->ODR ^= GPIO_ODR_ODR7;
            receivedPacket = 0;
        }
    }
#endif    
    
#ifdef SD_TEST
    // SD card init
    txData.pBuffer = tekst_init;
    txData.lengthAT = 16;
    UART_WriteP(&txData);
    if(SD_init())
    {
        // uspesno
        txData.pBuffer = tekst_complete;
        txData.lengthAT = 9;
        UART_WriteP(&txData);
    }
    else
    {
        // failed
        txData.pBuffer = tekst_fail;
        txData.lengthAT = 7;
        UART_WriteP(&txData);
        while(1);
    }


    for(i=0; i<256; ++i)
    {
        blokpodatkov[i]   = i;
        blokpodatkov[i+256] = i;
    }
    SD_writeBlock(0, blokpodatkov);
    SD_readBlock(0, blokbuffer);
    
    for(i=0; i<512; ++i)
    {
        txData.pBuffer = tekst_blok;
        txData.lengthAT = 33;
        UART_WriteP(&txData);
        if(blokpodatkov[i] == blokbuffer[i])
        {
            txData.pBuffer = tekst_da;
            txData.lengthAT = 3;
            UART_WriteP(&txData);
        }
        else
        {
            txData.pBuffer = tekst_ne;
            txData.lengthAT = 3;
            UART_WriteP(&txData);
        }
        
    }
		    while(1)
    {
        
        
        
    }
#endif
}
