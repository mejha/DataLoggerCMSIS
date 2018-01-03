/*
 * SPI.c - SPI driver file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains SPI functions
 *						
 * Description:			Contains initialization functions and
 *                      interface functions for SPI peripheral
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
 * Revisions:	- name: rev. date in a form dd.mm.yyyy
 *				  short description of the rev
 *				  
 */
 
 #include "stm32f10x.h"
 #include <cmsis_os.h>
 
 // mutex
 osMutexId id_mutex_SPI;
 
 
 
 #include "UART.h"
 
 
 
 // SPI3 for testing on easymx board
 // Pins:
 // 78 - PC10 - SPI3_SCK
 // 79 - PC11 - SPI3_MISO
 // 80 - PC12 - SPI3_MOSI
 // 84 - PD3  - CS
 
 
 
 void SPI_init()
 {
     // ze v GPIO init: AFIO->MAPR |= AFIO_MAPR_SPI3_REMAP;                // Required because of easy mx board
     
     SPI3->CR1 &= ~SPI_CR1_BR;
     SPI3->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_0);        // Baud rate = f_PCLK/64 = 390.625 kHz
     SPI3->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);       // CPOL = 0 && CPHA = 0
     SPI3->CR1 &= ~(SPI_CR1_DFF);                       // Data Frame Format = 8 bit
     SPI3->CR1 &= ~(SPI_CR1_LSBFIRST);                  // MSB first to be transmitted

     SPI3->CR2 = 0;                                     // No interrupts
     
     SPI3->CR2 &= ~SPI_CR2_SSOE;                        // SS output disable (upravljamo s software-om)
     SPI3->CR1 |= SPI_CR1_MSTR;                         // SPI master mode
     SPI3->CR1 &= ~(SPI_CR1_BIDIMODE | 
                    SPI_CR1_RXONLY);                    // Bidirectional half-duplex mode
                                                        // Set/clear SPI_CR1_BDIOE for Tx/Rx
                                                        // Check 25.3.5 for Tx and Rx procedures
     SPI3->CR1 &= ~(SPI_CR1_CRCEN);                     // CRC disable
     SPI3->CR1 |= SPI_CR1_SPE;                          // SPI enable
 }
 
 void SPI_Transmit(uint8_t *tx, uint8_t size)
 {
     // PD3 na low --> SDcard.c
     
     uint32_t n;
     
     for(n=0; n<size; ++n)
     {
         while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE);
         *((uint8_t *)(&(SPI3->DR))) = tx[n];
         //SPI3->DR = tx[n];
     }
     
     // ko koncas z vsemi pocakaj da je TXE=1 in nato BSY=0
     while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE);
     while((SPI3->SR & SPI_SR_BSY) == SPI_SR_BSY);
 }
 
 void SPI_Receive(uint8_t *rx, uint8_t size)
 {
     uint32_t n;
     
     for(n=0; n<size; ++n)
     {
        // pocakaj da je Tx buffer empty
         while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE); 
         
         // nalozi Tx buffer z dummy podatky, prenos se zacne samodejno
         *((uint8_t *)(&(SPI3->DR))) = 0xFF;
         
         // pocakaj, da je Rx buffer poln
         while((SPI3->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
         
         // preberi Rx vrednost iz data reg
         rx[n] = *((uint8_t *)(&SPI3->DR));
     }
     
     // pocakaj, da se Tx data poslje
     while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE);
     // pocakaj, da se Rx data prebere
     while((SPI3->SR & SPI_SR_RXNE));
     // pocakaj, dokler SPI dela svoje
     while((SPI3->SR & SPI_SR_BSY) == SPI_SR_BSY);
 }
 
 void SPI_TransmitReceive(uint8_t *tx, uint8_t *rx, uint8_t size)
 {
     // PD3 na low --> SDcard.c     
     
     uint32_t n;
     
     for(n=0; n<size; ++n)
     {
         // pocakaj da je Tx buffer empty
         while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE); 
         
         // nalozi Tx buffer s podatki, prenos se zacne samodejno
         *((uint8_t *)(&(SPI3->DR))) = tx[n];
         
         // pocakaj, da je Rx buffer poln
         while((SPI3->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
         
         // preberi Rx vrednost iz data reg
         rx[n] = *((uint8_t *)(&SPI3->DR));
     }
     
     // pocakaj, da se Tx data poslje
     while((SPI3->SR & SPI_SR_TXE) != SPI_SR_TXE);
     // pocakaj, da se Rx data prebere
     while((SPI3->SR & SPI_SR_RXNE));
     // pocakaj, dokler SPI dela svoje
     while((SPI3->SR & SPI_SR_BSY) == SPI_SR_BSY);
 }
 