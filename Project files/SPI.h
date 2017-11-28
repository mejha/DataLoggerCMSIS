/* 
 * SPI.h - SPI header file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains SPI function declarations
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	9.4.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	- (name: rev. date in a form dd.mm.yyyy
 *				  short description of the rev)
 *				  
 */
 
 #ifndef _SPI_H
 #define _SPI_H
 
 #include "stm32f10x.h"
  
 void SPI_init(void);
 void SPI_Transmit(uint8_t *tx, uint8_t size);
 void SPI_Receive(uint8_t *rx, uint8_t size);
 void SPI_TransmitReceive(uint8_t *tx, uint8_t *rx, uint8_t size);
 
 #endif
 