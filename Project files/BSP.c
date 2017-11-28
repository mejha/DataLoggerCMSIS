/**
  ******************************************************************************
  * @file    stm3210c_eval.c
  * @author  MCD Application Team
  * @version V6.0.2
  * @date    29-April-2016
  * @brief   This file provides a set of firmware functions to manage Leds, 
  *          push-button and COM ports for STM3210C_EVAL
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/******************************** LINK SD Card ********************************/

/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for 
  *         data transfer).
  * @retval None
  */
void SD_IO_Init(void)
{
  GPIO_InitTypeDef  gpioinitstruct;
  uint8_t counter;

  /* SD_CS_GPIO and SD_DETECT_GPIO Periph clock enable */
  SD_CS_GPIO_CLK_ENABLE();
  SD_DETECT_GPIO_CLK_ENABLE();

  /* Configure SD_CS_PIN pin: SD Card CS pin */
  gpioinitstruct.Pin    = SD_CS_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_PORT, &gpioinitstruct);

  /* Configure SD_DETECT_PIN pin: SD Card detect pin */
  gpioinitstruct.Pin    = SD_DETECT_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_IT_RISING_FALLING;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpioinitstruct);

  /* Enable and set SD EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(SD_DETECT_EXTI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DETECT_EXTI_IRQn);

  /*------------Put SD in SPI mode--------------*/
  /* SD SPI Config */
  SPIx_Init();
  
  /* SD chip select high */
  SD_CS_HIGH();
  
  /* Send dummy byte 0xFF, 10 times with CS high */
  /* Rise CS and MOSI for 80 clocks cycles */
  for (counter = 0; counter <= 9; counter++)
  {
    /* Send dummy byte 0xFF */
    SD_IO_WriteByte(SD_DUMMY_BYTE);
  }
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
void SD_IO_WriteByte(uint8_t Data)
{
  /* Send the byte */
  SPIx_Write(Data);
}

/**
  * @brief  Read a byte from the SD.
  * @retval The received byte.
  */
uint8_t SD_IO_ReadByte(void)
{
  uint8_t data = 0;
  
  /* Get the received data */
  data = SPIx_Read();

  /* Return the shifted data */
  return data;
}

/**
  * @brief  Send 5 bytes command to the SD card and get response
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @param  Response: Expected response from the SD card
  * @retval  HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef SD_IO_WriteCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Response)
{
  uint32_t counter = 0x00;
  uint8_t frame[6];

  /* Prepare Frame to send */
  frame[0] = (Cmd | 0x40); /* Construct byte 1 */
  frame[1] = (uint8_t)(Arg >> 24); /* Construct byte 2 */
  frame[2] = (uint8_t)(Arg >> 16); /* Construct byte 3 */
  frame[3] = (uint8_t)(Arg >> 8); /* Construct byte 4 */
  frame[4] = (uint8_t)(Arg); /* Construct byte 5 */
  frame[5] = (Crc); /* Construct CRC: byte 6 */
  
  /* SD chip select low */
  SD_CS_LOW();
    
  /* Send Frame */
  for (counter = 0; counter < 6; counter++)
  {
    SD_IO_WriteByte(frame[counter]); /* Send the Cmd bytes */
  }

  if(Response != SD_NO_RESPONSE_EXPECTED)
  {
    return SD_IO_WaitResponse(Response);
  }
  
  return HAL_OK;
}

/**
  * @brief  Wait response from the SD card
  * @param  Response: Expected response from the SD card
  * @retval  HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef SD_IO_WaitResponse(uint8_t Response)
{
  uint32_t timeout = 0xFFFF;
  uint8_t resp = 0;
  /* Check if response is got or a timeout is happen */
  resp = SD_IO_ReadByte();
  while ((resp != Response) && timeout)
  {
    timeout--;
    resp = SD_IO_ReadByte();
  }

  if (timeout == 0)
  {
    /* After time out */
    return HAL_TIMEOUT;
  }
  else
  {
    /* Right response got */
    return HAL_OK;
  }
}

/**
  * @brief  Send dummy byte with CS High
  * @retval None
  */
void SD_IO_WriteDummy(void)
{
    /* SD chip select high */
    SD_CS_HIGH();
    
    /* Send Dummy byte 0xFF */
    SD_IO_WriteByte(SD_DUMMY_BYTE);
}

#endif /* HAL_SPI_MODULE_ENABLED */
