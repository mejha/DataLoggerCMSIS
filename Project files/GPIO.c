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
     
//     AFIO->MAPR |= AFIO_MAPR_SPI3_REMAP;// Premostimo SPI3 na PC10, PC11, PC12 in PA4 pine
     
     // === Port A ===
     // SPI_NSS  (PA4)  -  Alternate function push-pull
//     GPIOA->CRL &= ~(GPIO_CRL_MODE);    // Clear all MODEx[1:0] bits; makes them inputs
//     GPIOA->CRL &= ~(GPIO_CRL_CNF);     // Clear all CNFx[1:0] bits; makes them analog
//     GPIOA->CRL |= GPIO_CRL_CNF4_1;     // GPIO is in alternate function push-pull
//     GPIOA->CRL |= GPIO_CRL_MODE4_0;    // Max. output speed 10MHz
     
     // JTAG/SWD are left as main functions after reset so nothing is done at this point
     
     // === Port C ===
     // SPI_SCK  (PC10) -  Alternate function push-pull
     // SPI_MOSI (PC12) -  Alternate function push-pull
     // SPI_MISO (PC11) -  Input, pull-up
//     GPIOC->CRH &= ~(GPIO_CRH_MODE);    // Clear all MODEx[1:0] bits; makes them inputs
//     GPIOC->CRH |= (GPIO_CRH_MODE10_0 |
//                    GPIO_CRH_MODE12_0);
//     GPIOC->CRH &= ~(GPIO_CRH_CNF);     // Clear all CNFx[1:0] bits; analog mode
//     GPIOC->CRH |= (GPIO_CRH_CNF10_1 |  // PC10 is in alternate function; push-pull 
//                    GPIO_CRH_CNF12_1 |  // PC12 is in alternate function; push-pull 
//                    GPIO_CRH_CNF11_1);  // PC11 is in input mode
//     GPIOC->ODR |= GPIO_ODR_ODR11;      // Enable pull-up on PC11
    
     
     // When LSE is enabled it has priority over GPIO on PC13 and PC14 
     
     
     // ******************** UART ********************
     
     // === Port C ===
     // UART1_TX (PC10)  - Alternate function push-pull
     // UART1_RX (PC11) - Input - pull-up
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
 