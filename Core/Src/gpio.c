/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "gvl.h"
#include <stdbool.h>

extern uint16_t liftcanMode;

ports inPorts[1][8] = {{
	{0, 0, DEBOUNCE, 0, TIME_MAX, 1},	// TO
	{0, 0, DEBOUNCE, 0, TIME_MAX, 1},	// TU
	{0, 0, DEBOUNCE, 0, TIME_MAX, 1}, 	// VA
	{0, 0, DEBOUNCE, 0, 0, 0}, 			// ZSE
	{0, 0, DEBOUNCE, 0, 0, 0}, 			// FWE
	{0, 0, DEBOUNCE, 0, 0, 0}, 			// SAE
	{0, 0, DEBOUNCE, 0, 0, 0}, 			// WE1
	{0, 0, DEBOUNCE, 0, 0, 0},			// WE2
}};

#define IN1_8_PORT 0
#define PW_KABINOWY 0xFF

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
uint8_t CheckedInputs;
CanAdress liftCanAdress;
GvlList global;
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EU_OUT2_Pin|EU2_OUT1_Pin|SPI2_CS_SDCARD_Pin|SPI2_FLASH_WP_Pin
                          |SPI2_FLASH_HOLD_Pin|RO_OUT6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_PD_GPIO_Port, SPI1_PD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2C2_WP_GPIO_Port, I2C2_WP_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI2_FLASH_CS_Pin|EO2_OUT4_Pin|EO_OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RU_OUT5_GPIO_Port, RU_OUT5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = EU_OUT2_Pin|EU2_OUT1_Pin|SPI1_PD_Pin|SPI2_CS_SDCARD_Pin
                          |SPI2_FLASH_WP_Pin|SPI2_FLASH_HOLD_Pin|RO_OUT6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = TO_IN1_Pin|TU_IN2_Pin|VA_IN3_Pin|ZSE_IN4_Pin
                          |UP_S1_BTT_Pin|ENT_S5_BTT_Pin|DN_S2_BTT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin
                           PAPin PAPin */
  GPIO_InitStruct.Pin = FWE_IN5_Pin|SAE_IN6_Pin|WE1_IN7_Pin|WE2_IN8_Pin
                          |LEFT_S3_BTT_Pin|RIGHT_S4_BTT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI1_INT_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI1_INT_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = I2C2_WP_Pin|SPI2_FLASH_CS_Pin|EO2_OUT4_Pin|EO_OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RU_OUT5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RU_OUT5_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void setOutputs(uint8_t outputs)
{
	if (outputs & 0x20) HAL_GPIO_WritePin(EU2_OUT1_GPIO_Port, EU2_OUT1_Pin, 1); else HAL_GPIO_WritePin(EU2_OUT1_GPIO_Port, EU2_OUT1_Pin, 0);
	if (outputs & 0x10) HAL_GPIO_WritePin(EU_OUT2_GPIO_Port,  EU_OUT2_Pin,  1); else HAL_GPIO_WritePin(EU_OUT2_GPIO_Port,  EU_OUT2_Pin,  0);
	if (outputs & 0x08) HAL_GPIO_WritePin(EO_OUT3_GPIO_Port,  EO_OUT3_Pin,  1); else HAL_GPIO_WritePin(EO_OUT3_GPIO_Port,  EO_OUT3_Pin,  0);
	if (outputs & 0x04) HAL_GPIO_WritePin(EO2_OUT4_GPIO_Port, EO2_OUT4_Pin, 1); else HAL_GPIO_WritePin(EO2_OUT4_GPIO_Port, EO2_OUT4_Pin, 0);
	if (outputs & 0x02) HAL_GPIO_WritePin(RU_OUT5_GPIO_Port,  RU_OUT5_Pin,  1); else HAL_GPIO_WritePin(RU_OUT5_GPIO_Port,  RU_OUT5_Pin,  0);
	if (outputs & 0x01) HAL_GPIO_WritePin(RO_OUT6_GPIO_Port,  RO_OUT6_Pin,  1); else HAL_GPIO_WritePin(RO_OUT6_GPIO_Port,  RO_OUT6_Pin,  0);
}

uint8_t readInputs(void)
{
	uint8_t input = 0;

	if (!HAL_GPIO_ReadPin(WE2_IN8_GPIO_Port, WE2_IN8_Pin)) 	input |= 0x80; else input &= 0x7F;
	if (!HAL_GPIO_ReadPin(WE1_IN7_GPIO_Port, WE1_IN7_Pin)) 	input |= 0x40; else input &= 0xBF;
	if (!HAL_GPIO_ReadPin(SAE_IN6_GPIO_Port, SAE_IN6_Pin)) 	input |= 0x20; else input &= 0xDF;
	if (!HAL_GPIO_ReadPin(FWE_IN5_GPIO_Port, FWE_IN5_Pin)) 	input |= 0x10; else input &= 0xEF;
	if (!HAL_GPIO_ReadPin(ZSE_IN4_GPIO_Port, ZSE_IN4_Pin)) 	input |= 0x08; else input &= 0xF7;
	if (!HAL_GPIO_ReadPin(VA_IN3_GPIO_Port,  VA_IN3_Pin)) 	input |= 0x04; else input &= 0xFB;
	if (!HAL_GPIO_ReadPin(TU_IN2_GPIO_Port,  TU_IN2_Pin)) 	input |= 0x02; else input &= 0xFD;
	if (!HAL_GPIO_ReadPin(TO_IN1_GPIO_Port,  TO_IN1_Pin)) 	input |= 0x01; else input &= 0xFE;

	return input;
}

void checkInputs(void)
{
//	uint8_t inputsINx = 0;
//
//	inputsINx = readInputs();
	CheckedInputs = sweepPort(IN1_8_PORT, readInputs(), CheckedInputs);
}

uint8_t sweepPort(uint8_t port, uint8_t inputState, uint8_t previousInput)
{
	uint8_t i, j, value, receivedData;
	static uint8_t mask[1] = {0xFF}; /*static jest wymagany do kasowania zablokowanego przycisku*/

	receivedData = previousInput;

	for (i = 0; i < 8; i++) /*glowna petla odczytu stanow wejsc INx*/
	{
		j = 1 << i;

		if (inputState & j)
			value = 1;
		else
			value = 0;

		if ((mask[port] & j) == 0)
			value = 0;

		//if ((ZEZWEZ & j)==0) wartosc=0;
		if (value == inPorts[port][i].value)
		{
			if (inPorts[port][i].time < inPorts[port][i].maxTime)
					inPorts[port][i].time++;

			if (inPorts[port][i].time == inPorts[port][i].maxTime)
			{
				inPorts[port][i].time++;

				if (inPorts[port][i].value == 1)
					receivedData |= j;
				else
					receivedData &= (~j);

				global.DataToSendCANReady = true;
			}
		}
		else
		{
			inPorts[port][i].time = 0;
			inPorts[port][i].value = value;
		}

		/*ponizszy warunek sprawdza czy przycisk jest zablokowany, jezeli tak - ignoruje go*/
		if (inPorts[port][i].status > 0)
		{
			if (inputState & j)
				value = 1;
			else
				value = 0;

			if (value == 1)
			{
				if (inPorts[port][i].push < inPorts[port][i].maxPushTime)
				{
					inPorts[port][i].push++;
				}
				else
				{
					mask[port] &= (~j);

					if (inPorts[port][i].status == 1)
					{
						//chceckBuzzer(5,1);
						inPorts[port][i].status = 2;

						if(liftcanMode != PW_KABINOWY)
							sendCanFrame(DIAGNOSTIC_DATA, ONE_BYTE, 0x05, 0, 0, 0);
					}
				}
			}
			else
			{
				if (inPorts[port][i].status == 2)
				{
					if(liftcanMode != PW_KABINOWY)
						sendCanFrame(DIAGNOSTIC_DATA, ONE_BYTE, 0x55, 0, 0, 0);
				}

				inPorts[port][i].status = 1;
				inPorts[port][i].push = 0;
				mask[port] |= j;
			}
		}
	}

	return receivedData;
}

/* USER CODE END 2 */
