/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void eveMsgMultiLine(int16_t startX, int16_t startY, int16_t font, int16_t lineSpacing, int16_t lineCount, ...);
void decodeCodeFloor(uint8_t* canFloorCode, uint8_t* flashing, uint8_t *errorDetect, uint8_t lang, uint8_t lastCode);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EU_OUT2_Pin GPIO_PIN_13
#define EU_OUT2_GPIO_Port GPIOC
#define EU2_OUT1_Pin GPIO_PIN_14
#define EU2_OUT1_GPIO_Port GPIOC
#define TO_IN1_Pin GPIO_PIN_0
#define TO_IN1_GPIO_Port GPIOC
#define TU_IN2_Pin GPIO_PIN_1
#define TU_IN2_GPIO_Port GPIOC
#define VA_IN3_Pin GPIO_PIN_2
#define VA_IN3_GPIO_Port GPIOC
#define ZSE_IN4_Pin GPIO_PIN_3
#define ZSE_IN4_GPIO_Port GPIOC
#define FWE_IN5_Pin GPIO_PIN_0
#define FWE_IN5_GPIO_Port GPIOA
#define SAE_IN6_Pin GPIO_PIN_1
#define SAE_IN6_GPIO_Port GPIOA
#define WE1_IN7_Pin GPIO_PIN_2
#define WE1_IN7_GPIO_Port GPIOA
#define WE2_IN8_Pin GPIO_PIN_3
#define WE2_IN8_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_INT_N_Pin GPIO_PIN_4
#define SPI1_INT_N_GPIO_Port GPIOC
#define SPI1_PD_Pin GPIO_PIN_5
#define SPI1_PD_GPIO_Port GPIOC
#define I2C2_WP_Pin GPIO_PIN_2
#define I2C2_WP_GPIO_Port GPIOB
#define SPI2_FLASH_CS_Pin GPIO_PIN_12
#define SPI2_FLASH_CS_GPIO_Port GPIOB
#define SPI2_CS_SDCARD_Pin GPIO_PIN_6
#define SPI2_CS_SDCARD_GPIO_Port GPIOC
#define SPI2_FLASH_WP_Pin GPIO_PIN_7
#define SPI2_FLASH_WP_GPIO_Port GPIOC
#define SPI2_FLASH_HOLD_Pin GPIO_PIN_8
#define SPI2_FLASH_HOLD_GPIO_Port GPIOC
#define UP_S1_BTT_Pin GPIO_PIN_9
#define UP_S1_BTT_GPIO_Port GPIOC
#define LEFT_S3_BTT_Pin GPIO_PIN_8
#define LEFT_S3_BTT_GPIO_Port GPIOA
#define RIGHT_S4_BTT_Pin GPIO_PIN_15
#define RIGHT_S4_BTT_GPIO_Port GPIOA
#define ENT_S5_BTT_Pin GPIO_PIN_10
#define ENT_S5_BTT_GPIO_Port GPIOC
#define DN_S2_BTT_Pin GPIO_PIN_11
#define DN_S2_BTT_GPIO_Port GPIOC
#define RO_OUT6_Pin GPIO_PIN_12
#define RO_OUT6_GPIO_Port GPIOC
#define RU_OUT5_Pin GPIO_PIN_2
#define RU_OUT5_GPIO_Port GPIOD
#define EO2_OUT4_Pin GPIO_PIN_3
#define EO2_OUT4_GPIO_Port GPIOB
#define EO_OUT3_Pin GPIO_PIN_4
#define EO_OUT3_GPIO_Port GPIOB
#define BUZZER_PWM_Pin GPIO_PIN_5
#define BUZZER_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define MENU_POSITION (((menuPosSel - 20) / (20)))
#define HEARTHBEAT_CAN 5000 //10s
#define RAM_G_LARGE_FONT_OFFSET 0
#define RAM_G_LARGE_NUMBER_OFFSET 180000
#define RAM_G_PL_FONT_OFFSET 240000

typedef enum{
	FIRST_FLASH_FONT,
	BUILT_FONT,
	BIG_ARTIFAKT_FONT,
	SMALL_PL_FONT,
	LARGE_NUMBER
} fonts;

#define IWDG_ON 1

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
