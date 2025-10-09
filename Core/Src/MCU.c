/**
 @file EVE_MCU_STM32.c
*/
/*
* ============================================================================
* History
* =======
* Nov 2019		Initial version
*
*
*
*
*
*
*
* (C) Copyright Bridgetek Pte Ltd
* ============================================================================
*
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
* http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
* ============================================================================
*/

#define PLATFORM_STM32_CUBE

#if defined(PLATFORM_STM32_CUBE)

#pragma message "Compiling " __FILE__ " for ST STM32_CUBE"

#define bswap16(x) (((x) >> 8) | ((x) << 8))
#define bswap32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) \
                  | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_def.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_rcc.h>
#include <stm32f0xx_hal_spi.h>

/* Replace with header file for target MCU */
//#include "stm32f0308_discovery.h"

#include <main.h>
#include <EVE.h>
#include <EVE_config.h>
#include <FT8xx.h>
#include <HAL.h>
#include <MCU.h>
#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/* SPI handler declaration */
extern SPI_HandleTypeDef hspi1;

#define FT81X_ENABLE
//SPI_HandleTypeDef hspi1;

void MCU_Init(void)
{
//	 GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//	  /* GPIO Ports Clock Enable */
//	  __HAL_RCC_GPIOF_CLK_ENABLE();
//	  __HAL_RCC_GPIOA_CLK_ENABLE();
//	  __HAL_RCC_GPIOC_CLK_ENABLE();
//	  __HAL_RCC_GPIOB_CLK_ENABLE();
//
//	  /*Configure GPIO pin Output Level */
//	  HAL_GPIO_WritePin(SPI1_PD_GPIO_Port, SPI1_PD_Pin, GPIO_PIN_SET);
//
//	  /*Configure GPIO pin Output Level */
//	  HAL_GPIO_WritePin(EPPROM_WP_GPIO_Port, EPPROM_WP_Pin, GPIO_PIN_RESET);
//
//	  /*Configure GPIO pin : PtPin */
//	  GPIO_InitStruct.Pin = SPI1_INT_N_Pin;
//	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	  HAL_GPIO_Init(SPI1_INT_N_GPIO_Port, &GPIO_InitStruct);
//
//	  /*Configure GPIO pin : PtPin */
//	  GPIO_InitStruct.Pin = SPI1_PD_Pin;
//	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	  HAL_GPIO_Init(SPI1_PD_GPIO_Port, &GPIO_InitStruct);
//
//	  /*Configure GPIO pin : PtPin */
//	  GPIO_InitStruct.Pin = EPPROM_WP_Pin;
//	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	  HAL_GPIO_Init(EPPROM_WP_GPIO_Port, &GPIO_InitStruct);
//
//	   hspi1.Instance = SPI1;
//	   hspi1.Init.Mode = SPI_MODE_MASTER;
//	   hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//	   hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
//	   hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//	   hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//	   hspi1.Init.NSS = SPI_NSS_SOFT;
//	   hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
//	   hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//	   hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//	   hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//	   hspi1.Init.CRCPolynomial = 7;
//	   hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//	   hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

	   /* USER CODE BEGIN SPI1_Init 2 */
}

void MCU_Setup(void)
{
#ifdef FT81X_ENABLE
#endif // FT81X_ENABLE
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); //lo
  //Nop();
}


// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET); //hi
  //Nop();
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
  HAL_GPIO_WritePin(SPI1_PD_GPIO_Port, SPI1_PD_Pin, GPIO_PIN_RESET); //lo                                                     // PD# line low
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
  HAL_GPIO_WritePin(SPI1_PD_GPIO_Port, SPI1_PD_Pin, GPIO_PIN_SET); //hi                                                      // PD# line high
}

// --------------------- SPI Send and Receive ----------------------------------
uint8_t MCU_SPIReadWrite8(uint8_t DataToWrite)
{
    
	uint8_t DataRead[4];
	uint8_t TxBuffer[4];
	
	 TxBuffer[0] = DataToWrite;
		
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)TxBuffer, (uint8_t *)DataRead, 1, 5000);

	// Note that this call to the STM32 HAL returns a status value which can be checked as shown below in order
	// to make the application more robust
#if 0
	switch(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)TxBuffer, (uint8_t *)DataRead, 1, 5000))
	{
    case HAL_OK:
      /* Communication is completed ___________________________________________ */
    case HAL_TIMEOUT:
      /* A Timeout Occur ______________________________________________________*/
      /* Call Timeout Handler */
      Timeout_Error_Handler();
      break;
      /* An Error Occur ______________________________________________________ */
    case HAL_ERROR:
      /* Call Timeout Handler */
      Error_Handler();
      break;
    default:
      break;
  }
#endif
	   

    return DataRead[0];
}

uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite)
{
    uint16_t DataRead = 0;
    DataRead = MCU_SPIReadWrite8((DataToWrite) >> 8) << 8;
    DataRead |= MCU_SPIReadWrite8((DataToWrite) & 0xff);
    return MCU_be16toh(DataRead);
}



uint32_t MCU_SPIReadWrite24(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;
    

     temp = (MCU_SPIReadWrite8((DataToWrite) >> 24)); //****
    DataRead |= (temp<<24);

     temp = (MCU_SPIReadWrite8((DataToWrite) >> 16));
    DataRead |= (temp<<16);
     temp = (MCU_SPIReadWrite8((DataToWrite) >> 8));
    DataRead |= (temp<<8);

    return MCU_be32toh(DataRead);
}


uint32_t MCU_SPIReadWrite32(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;

	temp = (MCU_SPIReadWrite8((DataToWrite) >> 24)); 
    DataRead |= (temp << 24);
     temp = (MCU_SPIReadWrite8((DataToWrite) >> 16)); 
    DataRead |= (temp << 16);      
    DataRead |= (MCU_SPIReadWrite8((DataToWrite) >> 8) << 8);      
    DataRead |= (MCU_SPIReadWrite8(DataToWrite) & 0xff); 
 
    return MCU_be32toh(DataRead);
}

void MCU_Delay_20ms(void)
{
    HAL_Delay(20);
	}

void MCU_Delay_500ms(void)
{
    uint8_t dly = 0;

    for(dly =0; dly < 100; dly++)
    {
        HAL_Delay(20);
    }
}



// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
	uint8_t DataRead = 0;

	DataRead = MCU_SPIReadWrite8(0);
    
	return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
	MCU_SPIReadWrite8(DataToWrite);
}

uint16_t MCU_SPIRead16(void)
{
	uint16_t DataRead = 0;

	DataRead = MCU_SPIReadWrite16(0);

	return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
	MCU_SPIReadWrite16(DataToWrite);
}

uint32_t MCU_SPIRead24(void)
{
	uint32_t DataRead = 0;

	DataRead = MCU_SPIReadWrite24(0);

	return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
	MCU_SPIReadWrite24(DataToWrite);
}

uint32_t MCU_SPIRead32(void)
{
	uint32_t DataRead = 0;

	DataRead = MCU_SPIReadWrite32(0);

	return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
	MCU_SPIReadWrite32(DataToWrite);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
	//spi_writen(SPIM, DataToWrite, length);

    uint16_t DataPointer = 0;   
    DataPointer = 0;

    while(DataPointer < length)
    {
        MCU_SPIWrite8(DataToWrite[DataPointer]);                                       // Send data byte-by-byte from array
        DataPointer ++;
    }
}

uint16_t MCU_htobe16 (uint16_t h)
{
    return h;
}

uint32_t MCU_htobe32 (uint32_t h)
{
    return h;
}

uint16_t MCU_htole16 (uint16_t h)
{

        return bswap16(h); 
}

uint32_t MCU_htole32 (uint32_t h)
{

        return bswap32(h);
}

uint16_t MCU_be16toh (uint16_t h)
{
    return h;
}
uint32_t MCU_be32toh (uint32_t h)
{
     return h;
}

uint16_t MCU_le16toh (uint16_t h)
{

        return bswap16(h); 
}

uint32_t MCU_le32toh (uint32_t h)
{
        return bswap32(h);

}

#if 0
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Timeout_Error_Handler(void)
{
  /* Toggle LED4 on */
  while(1)
  {
    BSP_LED_On(LED4);
    HAL_Delay(500);
    BSP_LED_Off(LED4);
    HAL_Delay(500);
  }
}
#endif

#if 0
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED4 on */
  BSP_LED_On(LED4);
  while(1)
  {
  }
}
#endif

#endif /* defined (PLATFORM_STM32) */
