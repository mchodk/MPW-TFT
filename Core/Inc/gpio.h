/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <buzzer.h>
#include "can.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define DEBOUNCE 40 // ms
#define TIME_MAX 20000 // ms

typedef enum{
	UP_BUTTON,
	DN_BUTTON,
	LEFT_BUTTON,
	RIGHT_BUTTON,
	ENT_BUTTON,
	DEBOUNCE_MS = 30
} button;

typedef struct
{
	uint8_t 	value;			// stan wejscia
	uint8_t 	time;			// czas trwania aktualnego stau wejscia
	uint8_t 	maxTime;		// filtr zmiany stanu wejscia
	uint16_t 	push;			// czas aktywnosci wejscia
	uint16_t 	maxPushTime;	// maksymalny czas aktywnosci wejscia
	uint8_t 	status;			// stan kontroli wcisniecia
} ports;

extern ports inPorts[1][8];
extern uint8_t CheckedInputs;
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void setOutputs(uint8_t outputs);
uint8_t readInputs(void);
uint8_t sweepPort(uint8_t port, uint8_t variable, uint8_t data);
void checkInputs(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

