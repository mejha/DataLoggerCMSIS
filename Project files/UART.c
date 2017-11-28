/* 
 * UART.c - UART driver file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains UART functions
 *						
 * Description:			Contains initialization functions and
 *                      interface functions for UART peripheral
 *                      of the STM32F107
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	4.6.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	
 *				  
 */

#include "stm32f10x.h"
#include "UART.h"

void UART_init()
{
//    RCC->APB2RSTR |= RCC_APB1RSTR_UART4RST;    // reset USART1 peripheral (clear all regs and set them to defaul values
//    RCC->APB2RSTR &= ~RCC_AP12RSTR_UART4RST;   // clr reset flag
    
    UART4->CR1 |= USART_CR1_UE;                // Enable USART output and prescaler (works all the time)
    //UART4->CR1 &= ~USART_CR1_M;                // M=0 --> 8-bit data len
    UART4->CR1 |= USART_CR1_M;                 // M=1 --> 9-bit data len
    
		//UART4->CR1 &= ~USART_CR1_PCE;
    UART4->CR1 |= USART_CR1_PCE;               // Parity control enable
    UART4->CR1 &= ~USART_CR1_PS;               // Even parity: Parity Selection PS == 0
    
    UART4->CR2 &= ~USART_CR2_STOP;             // STOP[1:0] = 0 == 1 stop bit
    
    UART4->CR3 = 0;                            // clear interrupt enables and other unused stuff
    
    UART4->BRR = (uint16_t)~(0xFFFF);
    UART4->BRR |= ( (40<<4) | (11<<0) );       // BR = 162 + 12/16   --->  baudrate = 9600  (27.3.4 in RM)
//UART4->BRR |= ( (52<<4) | (1<<0) );
//UART4->BRR |= 0x341;
    
    UART4->CR2 &= ~USART_CR2_CLKEN;             // CLK not used
    
    UART4->CR1 |= (USART_CR1_TE |               // Tx enable
                    USART_CR1_RE);              // Rx enable
    
    // Bit 7 TXEIE: TXE interrupt enable
// 1: A USART interrupt is generated whenever TXE=1 in the USART_SR register

//	UART4->CR1 |= (0x1<<7);
	


//	Bit 6 TCIE: Transmission complete interrupt enable
	// 1: A USART interrupt is generated whenever TC=1 in the USART_SR register
	
//	UART4->CR1 |= (0x1<<6);
    
    // Bit 5 RXNEIE: RXNE interrupt enable
// 1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_SR register
//	UART4->CR1 |= (0x1<<5);
}

void UART_Write(UART_Data *tx)
{
    uint32_t i;
    for(i=0; i<tx->length; ++i)
    {
        while(!(UART4->SR & USART_SR_TXE));        // wait uuntil Tx reg is empty
        
        UART4->DR = tx->buffer[i] & 0xFF;
        
        while(!(UART4->SR & USART_SR_TXE));
    }
    
    
    // ************************** lahko posljes se break char ali kaj podobnega ***********************************
    
    
    while(!(UART4->SR & USART_SR_TC));         // wait for transmission complete
    UART4->SR &= ~USART_SR_TC;
}

void UART_WriteP(UART_Data *tx)
{
    uint32_t i;
    for(i=0; i<tx->length; ++i)
    {
        while(!(UART4->SR & USART_SR_TXE));        // wait uuntil Tx reg is empty
        
        UART4->DR = tx->pBuffer[i] & 0xFF;
    }
    
    
    // ************************** lahko posljes se break char ali kaj podobnega ***********************************
    
    
    while(!(UART4->SR & USART_SR_TC));         // wait for transmission complete
    UART4->SR &= ~USART_SR_TC;
}


void UART_Read(UART_Data *rx)
{
    uint8_t flagDAreceived = 0;
    
    uint8_t beri = 1;

    uint32_t n = 0;
    uint32_t SRvalue = 0;
    resetRxStatus(&(rx->rxStatus), 0, 0, 0, 0, 0);
    rx->length = 0;                                 // reset len vrednost

    while(beri) 
    {   
        while(!(UART4->SR & USART_SR_RXNE));       // wait for Rx Not Empty 
        
        rx->buffer[n] = UART4->DR & 0xFF;          // shrani vrednost v Rx Data Reg v buffer[i]
			
        ++n;																	 		// inkrementiraj
			
        if(n>1)                                    // memorry protection: gledamo za en bit nazaj
        {
            if( (rx->buffer[n-2] == 0x0D) &&       // zadnja dva znaka: 0xD 0xA
                (rx->buffer[n-1] == 0x0A))
            {
                if(flagDAreceived==0)              // 0xD 0xA se je pojavilo prvic?
                {
                    n = 0;                          // zacni polniti buffer od zacetka (0xD 0xA itak ne rabimo)
                    flagDAreceived = 1;						
                }
                else                               // 0xD 0xA se je ze pojavil
                {
                    beri = 0;
                }
            }
        }
        
        // da prihranis na casu uporabi: valueSR |= USART->SR in na koncu pogledas, ali je prislo kdaj vmes do napake ali ne
        SRvalue |= UART4->SR;                                             
        
        if(n > UART_TX_RX_BUFFER_LEN)               // zascita, ce sprejmemo vec znakov, kot pa imamo rezerviranega prostora
        {
            rx->rxStatus.bufferLenExceeded = 1;     // nismo sprejeli celotnega sporocila (zaradi pomanjaknja prostora v bufferju)
            --n;
            beri = 0;
        }     
    }
    
    if(SRvalue & USART_SR_PE)                       // Parity Error se je med sprejemanjem pojavil
    {
        rx->rxStatus.parityError = 1;
    }
    if(SRvalue & USART_SR_FE)                       // Frame Error se je med sprejemanjem pojavil 
    {                                               // ( break character (all zeros), noise error ali de-sync zaznan )
        rx->rxStatus.breakCharDetected = 1;
    }
    
    rx->length = n;
    
    //if(currentRx.parityError) {...?...}           // zahtevaj ponovitev trnasmicije
    
    //if(currentRx.bufferLenExceeded) {...?...}     // velikost bufferja je manjsa od dolzine sprejetega sporocila --> nismo sprejeli celotnega sporocila
}

void UART_ReadReq(UART_Data *rx)
{
    
    uint8_t beri = 1;

    uint32_t n = 0;
    uint32_t SRvalue = 0;
    resetRxStatus(&(rx->rxStatus), 0, 0, 0, 0, 0);
    rx->length = 0;                                 // reset len vrednost

    while(beri) 
    {   
        while(!(UART4->SR & USART_SR_RXNE));       // wait for Rx Not Empty 
        
        rx->buffer[n] = UART4->DR & 0xFF;          // shrani vrednost v Rx Data Reg v buffer[i]
			
        ++n;																	 		// inkrementiraj
			
        if(n>3)                                    // memorry protection: gledamo za stiri bite nazaj
        {
            if( (rx->buffer[n-4] == 0x0D) &&       // tretji in cetrti znak: 0xD 0xA
                (rx->buffer[n-3] == 0x0A) &&
                (rx->buffer[n-2] == 0x0D) &&       // zadnja dva znaka: 0xD 0xA
                (rx->buffer[n-1] == 0x0A))
            {
                beri = 0;
            }
        }
        
        // da prihranis na casu uporabi: valueSR |= USART->SR in na koncu pogledas, ali je prislo kdaj vmes do napake ali ne
        SRvalue |= UART4->SR;                                             
        
        if(n > UART_TX_RX_BUFFER_LEN)               // zascita, ce sprejmemo vec znakov, kot pa imamo rezerviranega prostora
        {
            rx->rxStatus.bufferLenExceeded = 1;     // nismo sprejeli celotnega sporocila (zaradi pomanjaknja prostora v bufferju)
            --n;
            beri = 0;
        } 
    }
    
    if(SRvalue & USART_SR_PE)                       // Parity Error se je med sprejemanjem pojavil
    {
        rx->rxStatus.parityError = 1;
    }
    if(SRvalue & USART_SR_FE)                       // Frame Error se je med sprejemanjem pojavil 
    {                                               // ( break character (all zeros), noise error ali de-sync zaznan )
        rx->rxStatus.breakCharDetected = 1;
    }
    
    rx->length = n;
    
    //if(currentRx.parityError) {...?...}           // zahtevaj ponovitev trnasmicije
    
    //if(currentRx.bufferLenExceeded) {...?...}     // velikost bufferja je manjsa od dolzine sprejetega sporocila --> nismo sprejeli celotnega sporocila
}

void resetRxStatus(UART_Status *status, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e)
{
    status->breakCharDetected = a;
    status->bufferLenExceeded = b;
    status->EOTChar           = c;
    status->parityError       = d;
    status->ESPTxEnd          = e;
}
