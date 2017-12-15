#include <cmsis_os.h>
#include <stdio.h>

#include "stm32f10x.h"
#include "RCC.h"
#include "GPIO.h"
#include "SPI.h"
#include "UART.h"
#include "ESP.h"
#include "SDcard.h"

#define F_SYSCLK                25000000UL          // Cortex core clock
#define F_APB2CLK               (F_SYSCLK/4)        // GPIO clock

// thread
osThreadId id_main;

// mutex
osMutexId id_mutex_uart;

//#define ESP_TEST
//#define UART_TEST
//#define SPI_TEST
#define SD_TEST

int main()
{
    // var
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
    
    
    static UART_Data rxData, txData;        // Ce ni static se ob klicu funkcije UART_Read (kateri posljemo kazalec na strukturo kot argument),
                                            // takoj ob vstopu v funkcije preocesor skoci v HardFault_Handler
                                            // razlog je v optimizaciji: brez static je veljavnost variable samo v funkciji, kjer je bila deklarirana
                                            // Ce uporabimo static, se vrednost spremenljivke ohranja med funkcijami
                                            // https://blog.frankvh.com/2011/12/07/cortex-m3-m4-hard-fault-handler/    --> Hard Fault Handle Uasge
    
    
    // === Initialization ===    
    // Reset and Clock Control
    RCC_init();
    
    // GPIO init
    GPIO_init();
    
    // SPI init
    SPI_init();
    
    // UART init
    UART_init();
    
    // ESP init
    //ESP_init();
    
    // SD card init
    //SD_init(); - se klice nizje
    

#ifdef ESP_TEST

    while(1)
    {
        ESP_check_request();
    }

#endif

#ifdef UART_TEST
 
    while(1)
    {
        txData.pBuffer = jst;
        txData.length = 5;
        UART_WriteP(&txData);
        UART_Read(&rxData);
        UART_Write(&rxData);            //echo
        GPIOD->ODR ^= GPIO_ODR_ODR4;
    }
#endif
 
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
    txData.length = 16;
    UART_WriteP(&txData);
    if(SD_init())
    {
        // uspesno
        txData.pBuffer = tekst_complete;
        txData.length = 9;
        UART_WriteP(&txData);
    }
    else
    {
        // failed
        txData.pBuffer = tekst_fail;
        txData.length = 7;
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
        txData.length = 33;
        UART_WriteP(&txData);
        if(blokpodatkov[i] == blokbuffer[i])
        {
            txData.pBuffer = tekst_da;
            txData.length = 3;
            UART_WriteP(&txData);
        }
        else
        {
            txData.pBuffer = tekst_ne;
            txData.length = 3;
            UART_WriteP(&txData);
        }
        
    }
    
    while(1)
    {
        
        
        
    }
#endif
}
