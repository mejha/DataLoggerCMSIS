/* 
 * SDcard.c - SD card driver file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains SD card functions
 *						
 * Description:			Special procedures need to be undertaken 
 *                      when communicating with (micro) SD card.
 *                      This driver contains definitions of all 
 *                      the functions needed for an intuitive way
 *                      of communicating with the SD card.
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	10.4.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	- 
 *				  
 *				  
 */
 
#include "stm32f10x.h"
#include "SPI.h"
#include "SDcard.h"

#define SD_CS_PIN       GPIO_ODR_ODR3


uint8_t SD_init()
{
    uint32_t cnt;
    uint8_t status;
    uint8_t buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    
    // pocakaj vsaj 1ms po power-on
    for(cnt=0; cnt<25000; ++cnt);
    
    SD_deselectCard();
    // vsaj 74 urinih ciklov morata biti MOSI in CS high
    SPI_Transmit(buffer, 4);
    SPI_Transmit(buffer, 4);
    SPI_Transmit(buffer, 4);
    
    for(cnt=0; cnt<125000; ++cnt);  // 5ms cakanja
    
    SD_selectCard();
    
    // pocakaj, da gre kartica v idle state v SPI nacinu
    while((status = SD_cardCommand(CMD0, 0)) != R1_IDLE_STATE);
    
    // preveri verzijo
    if(SD_cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_CMD)
    {
        SD_deselectCard();
        return 0;
    }
    else
    {
        SPI_Receive(buffer, 4);
        if(buffer[3] != 0xAA)
        {
            return 0;
        }
    }
    
    SD_cardCommand(CMD55,0);
    status = SD_cardCommand(ACMD41, 0x40000000);        // najprej probamo ACMD41
    while(status != 0x00)
    {
        status = SD_cardCommand(CMD1, 0);               // ce ne gre, probamo CMD1 --> podpiramo vse kartice
    }
    
    // za checkiranje, ali gre za SD kartico z block naslavljanjem
    if(SD_cardCommand(CMD58, 0))
    {
        SD_deselectCard();
        return 0;
    }
    
    // set R/W block to 512B
    SD_cardCommand(CMD16, 0x00000200);
    SPI_Receive(buffer, 4);
    
    SD_deselectCard();
    return 1;
}

uint8_t SD_cardCommand(uint8_t cmd, uint32_t arg)
{
    uint8_t res = 0xFF;
    uint8_t cnt = 20;
    uint8_t buffer[6];
    buffer[0] = (uint8_t)(cmd | 0x40);
    buffer[1] = (uint8_t)((arg>>24) & 0xFF);
    buffer[2] = (uint8_t)((arg>>16) & 0xFF);
    buffer[3] = (uint8_t)((arg>> 8) & 0xFF);
    buffer[4] = (uint8_t)(arg       & 0xFF);
    buffer[5] = (uint8_t)0xFF;

    if(cmd == CMD0)
    {
        buffer[5] = 0x95;
    }
    else if(cmd == CMD8)
    {
        buffer[5] = 0x87;
    }
    
    SD_selectCard();
    SD_waitUntilReady();           // pocakaj, da kartica ni busy
    
    SPI_Transmit(buffer, 6);
   
    while((res & 0x80) && cnt)      // pocakaj na odgovor
    {
        SPI_Receive(&res, 1);
        --cnt;
    }
    
    return res;
}

uint8_t SD_readBlock(uint32_t addr, uint8_t *buffer)
{
    uint8_t temp = 0xFF;
    
    if(SD_cardCommand(CMD17, addr))
    {
        // error
        SD_deselectCard();
        return 0;
    }
    
    while(temp == 0xFF)
    {
        SPI_Receive(&temp, 1);
    }
    SPI_Receive(buffer, 512);
    SPI_Receive(&temp, 1);
    SPI_Receive(&temp, 1);
    
    SD_deselectCard();
    return 1;
    
}

uint8_t SD_writeBlock(uint32_t addr, uint8_t *buffer)
{
    uint8_t temp = DATA_START_BLOCK;
    
    if(SD_cardCommand(CMD24, addr))
    {
        SD_deselectCard();
        return 0;
    }
    
    // zapisipodatke
    SPI_Transmit(&temp, 1);     // token
    SPI_Transmit(buffer, 512);  // podatki
    temp = 0xFF;
    SPI_Transmit(&temp,1);
    SPI_Transmit(&temp,1);
    
    SPI_Receive(&temp, 1);
    if((temp & DATA_RES_MASK) != DATA_RES_ACCEPTED)
    {
        SD_deselectCard();
        return 0;
    }
    
    SD_waitUntilReady();   // pocakaj, da se zapisovanje v flash konca
    
    /*
    if(SD_cardCommand(13, 0))
    {
        //error
        SD_deselectCard();
        return 0;
    }
    SPI_Receive(&temp, 1);
    if(temp)
    {
        // error
        SD_deselectCard();
        return 0;
    }
    */
    
    SD_deselectCard();
    
    return 1;
}

uint8_t SD_readReg(uint8_t cmd, void* buf)
{
    uint8_t temp = 0xFF;
    
    uint8_t *ptr = (uint8_t *)buf;
    
    if(SD_cardCommand(cmd, 0))
    {
        SD_deselectCard();
        return 0;
    }
    
    while(temp == 0xFF)
    {
        SPI_Receive(&temp, 1);
    }
    
    SPI_Receive(ptr, 16);
    SPI_Receive(&temp, 1); // CRC1
    SPI_Receive(&temp, 1); // CRC2
    
    return 1;
}

void SD_selectCard()
{
    GPIOD->ODR &= ~SD_CS_PIN;
}

void SD_deselectCard()
{
    GPIOD->ODR |= SD_CS_PIN;
}

void SD_waitUntilReady()
{
    uint8_t rx = 0;
    uint8_t tx = 0xFF;
    while(rx != 0xFF)
    {
        SPI_TransmitReceive(&tx, &rx, 1);
    }
}
