#include <cmsis_os.h>
#include <stdio.h>

#include "stm32f10x.h"
#include "RCC.h"
#include "GPIO.h"
#include "SPI.h"
#include "UART.h"
#include "ESP.h"

#define F_SYSCLK                25000000UL          // Cortex core clock
#define F_APB2CLK               (F_SYSCLK/4)        // GPIO clock

// thread
osThreadId id_main;

// mutex
osMutexId id_mutex_uart;

#define ESP_TEST
//#define UART_TEST
//#define SPI_TEST


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
    //SPI_init();
    
    // UART init
    UART_init();
    
    // ESP init
    ESP_init();
    


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
}
