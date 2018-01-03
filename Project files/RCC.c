/* 
 * RCC.c - RCC driver file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains RCC functions
 *						
 * Description:			Contains Reset and clock control 
 *                      initialization functions for STM32F107
 *                      (conectivity line device)
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
 
/*
 *     Reset and Clock Control initialization
 */
void RCC_init()
{
    RCC->CFGR    &= ~RCC_CFGR_SW;                    // majprej izberes source clock, ker HSE ne mores izklopiti, ce ze enkrat poganja kakrsen koli modul/periferijo
    RCC->CFGR    |= RCC_CFGR_SW_HSE;                 // Use HSE as SYSCLK source
//RCC->CFGR    |= RCC_CFGR_SW_HSI;
    RCC->CR      |= (RCC_CR_HSEON | RCC_CR_HSION);   // Enable High-Speed-External and High-Speed-Internal Oscillators (HSE: SYSCLK source, HSI: Flash CLK)
//RCC->CR      |= RCC_CR_HSION;
//RCC->CR      &= ~RCC_CR_HSEON;
    RCC->CFGR    &= ~(RCC_CFGR_HPRE);                // AHB prescaler = 1
    RCC->CFGR    &= ~RCC_CFGR_PPRE1;
    RCC->CFGR    |= RCC_CFGR_PPRE1_DIV4;             // APB1DIV: Divide HSE=25MHz by 4 (boljsa nastavitev baude rate za UART; vezani tudi)
    RCC->CFGR    &= ~(RCC_CFGR_PPRE2);               // APB2DIV = 1
    RCC->APB2ENR |= ( RCC_APB2ENR_IOPBEN |           // Enable I/O Port B
                      RCC_APB2ENR_IOPCEN );          // Enable I/O Port C  
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
//    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;              // Enable SPI 3
//    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;            // Enable USART1
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;             // Enable UART4
}
 
 