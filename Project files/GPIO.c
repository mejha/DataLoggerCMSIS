/*
 * GPIO.c - GPIO driver file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains GPIO functions
 *						
 * Description:			Contains initialization functions and
 *                      interface functions for GPIO peripheral
 *                      of the STM32F107
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	4.4.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	- (name: rev. date in a form dd.mm.yyyy
 *				  short description of the rev)
 *				  
 */
 
 #include "stm32f10x.h"
 
 void GPIO_init()
 {
     
     // ******************** SPI ********************
     // Disable JTAG (shared with SPI pins) and enable SWI debug
     AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
     
     // === Port B ===
     // SCK - PB3
     // MISO - PB4
     // MOSI - PB5
     GPIOB->CRL |= GPIO_CRL_MODE3_0 | GPIO_CRL_MODE5_0;     // 01: outputs (max speed  = 10 MHz)
     GPIOB->CRL &= ~GPIO_CRL_MODE4;                         // 00: input
     GPIOB->CRL |= GPIO_CRL_CNF3_1 | GPIO_CRL_CNF5_1;       // 10: AF push-pull
     GPIOB->CRL |= GPIO_CRL_CNF4_0;                         // 01: floating input
     
     // === Port D ===
     // CS - PD3
     GPIOD->CRL |= GPIO_CRL_MODE3_0;     // 01: output (max speed = 10 MHz)
     GPIOD->CRL &= ~GPIO_CRL_CNF3;       // 00: GP push-pull
    
     
     // When LSE is enabled it has priority over GPIO on PC13 and PC14 
     
     
     // ******************** UART ********************
     
     // === Port C ===
     // UART4_TX (PC10) - Alternate function push-pull
     // UART4_RX (PC11) - Input - pull-up
     GPIOC->CRH &= ~(GPIO_CRH_CNF10 |    // clear CNF[1:0] for pin 9 and pin 10
                     GPIO_CRH_CNF11);
     GPIOC->CRH |= GPIO_CRH_CNF10_1;     // AF push-pull
     GPIOC->CRH |= GPIO_CRH_CNF11_1;     // Input  PU
     GPIOC->ODR |= GPIO_ODR_ODR11;       // pull-up
     GPIOC->CRH &= ~(GPIO_CRH_MODE10 |   
                     GPIO_CRH_MODE11);   // input
     GPIOC->CRH |= GPIO_CRH_MODE10;      // fastest output speed (50 MHz)
     
     
     // ******************** GPIO ********************
     
     // === Port B ===
     GPIOB->CRL &= ~(GPIO_CRL_MODE);     // max speed of 50MHz
     GPIOB->CRL |= GPIO_CRL_MODE4;
     GPIOB->CRL &= ~(GPIO_CRL_CNF);      // CNF1=0 (general purpose outputs), CNF0=0 (push-pull)
     
 }
 