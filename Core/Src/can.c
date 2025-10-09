/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */

#define DETECT_LS2020_FAIL 3
#define FLOOR_CODE 126
#define TERMINAL_ADRESS 0x1FA
#define PW_KABINOWY 0xFF
#define ACTIVE 2

extern void liftcanReset(uint8_t state);

volatile uint8_t expresstravel = 0;

extern uint16_t liftcanMode;

CAN_HandleTypeDef hcan;
CAN_TxHeaderTypeDef pHeader;
CAN_RxHeaderTypeDef pRxHeader;
uint32_t TxMailbox;
//uint8_t minute, hours;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 24;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* CAN clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
  /* USER CODE BEGIN CAN_MspInit 1 */

  /* USER CODE END CAN_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN interrupt Deinit */
    HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint8_t setupCANFilter (uint16_t sideAdress)
{
	CAN_FilterTypeDef  sFilterConfig;

	uint8_t state = CAN_INIT;

	//uint16_t adressForReceiveTimestamp = 0x1FA;
	if(liftCanAdress.TxAdress == PW_KABINOWY)
	{
		liftCanAdress.RxAdress 	 	= 0x002;
		liftCanAdress.TxAdress 	 	= 0x202;
		liftCanAdress.DiagTxAdress	= 0x22E;
		liftCanAdress.DisplayAdress = 0x600;
	}
	else
	{
		liftCanAdress.RxAdress 	 	= (sideAdress & 0x1F) << 3;
		liftCanAdress.TxAdress 	 	= liftCanAdress.RxAdress | 0x400;
		liftCanAdress.DiagTxAdress	= liftCanAdress.RxAdress | 0x004;
		liftCanAdress.DisplayAdress = 0x600;

		if (DETECT_PASSAGE_SIDE)
		{
			liftCanAdress.TxAdress 	 	|= 0x001;
			liftCanAdress.DiagTxAdress  |= 0x001;
			liftCanAdress.RxAdress 	 	|= 0x001;
			liftCanAdress.DisplayAdress |= 0x001;
		}
	}

	sFilterConfig.FilterMode 			= CAN_FILTERMODE_IDLIST;
    sFilterConfig.FilterScale 			= CAN_FILTERSCALE_16BIT;

	sFilterConfig.FilterIdHigh 			= (liftCanAdress.RxAdress << 5);
	sFilterConfig.FilterIdLow 			= (liftCanAdress.RxAdress << 5);//(liftCanAdress.RxAdress << 5);
    //sFilterConfig.FilterIdLow 			= (newAdress << 5);

    sFilterConfig.FilterMaskIdHigh 		= (liftCanAdress.DisplayAdress << 5);
    sFilterConfig.FilterMaskIdLow 		= (liftCanAdress.DisplayAdress << 5);

    sFilterConfig.FilterFIFOAssignment 	= CAN_FILTER_FIFO0;
    sFilterConfig.FilterActivation 		= ENABLE;
    sFilterConfig.FilterBank 			= 0;

    state = HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
    state = HAL_CAN_Start(&hcan);
    state = HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	if(state == HAL_OK)
		return CAN_SETUP_OK;
	else
		return CAN_NOT_READY;
}

uint8_t liftcanParser (uint8_t canDLC, uint8_t canByte0, uint8_t canByte1, uint8_t canByte2, uint8_t canByte3)
{
	uint8_t canCommand = 0; /*jezeli mamy -O0 volatile mozna darowac*/
	/* mozna dac tutaj strukture zmiennych ktora bedzie zwracana do maina*/

	//if(canID != TERMINAL_ADRESS)
	//{
		if (canDLC == 1) /*cos tam*/
		{
			canCommand = canByte0;

			if ((canCommand & 0x80) == 0)
			{
				global.SetOutpus = canCommand;
				global.OutputReady = true;
			}
			else
			{
				switch (canCommand & 0x7F)
				{
				case FLASH_EO_EU:			global.flashEoEu = true;		 	 					break;
				case CLEAR_CALL_ACK		:	global.SetOutpus &= 0xFC; global.OutputReady = true; 	break;
				case SYSTEM_RESET		:	HAL_NVIC_SystemReset(); 			 					break;
				case DATA_READY			:	global.DataToSendCANReady = true;	 					break;
				case DISPLAY_OFF		:	global.displayOff = true; 								break;
				case DISPLAY_ON			:	global.displayOff = false;								break;
				case REMOTE_BUZZER_ON	:	if(liftcanMode != PW_KABINOWY) global.buzzerRunState = 1; break;
				case REMOTE_BUZZER_OFF	:	if(liftcanMode != PW_KABINOWY) global.buzzerRunState = 2; break;
				}
			}
		}

		if (canDLC == 2) /*odbior danych z LS2020 SEG1 | SEG2*/
		{
			if ((global.CanByteSeg1 < FLOOR_CODE) && (global.CanByteSeg2 < DETECT_LS2020_FAIL))
				global.CanByteSeg3 = global.CanByteSeg1; /*W przypadku wystapienia usterki dxiwgu, CanByteSeg3 przechowuje ostatnia pozycje dzwigu*/

			if(canByte0 == 113) //ramka B0: 113 - jazda specjalna
				expresstravel = ACTIVE;
			if(expresstravel > 0 && canByte0 != 113) // kazda inna ramka z pietrem lub bledem
				expresstravel--;

			global.CanByteSeg1 = canByte0;
			global.CanByteSeg2 = canByte1;
			global.displayOff = false; //?
			global.displayReady = 5; //?
		}

		if (canDLC == 4) /*polecenia funkcyjne dla modulu*/
		{
			canCommand = canByte0;

			/*jasnosc wyswietlacza*/
			if (canCommand == 3)
			{
				if (!global.canCommandData)
				{
					global.canCommandReady = canCommand;
					global.canCommandData = canByte1;
				}
			}

			/*jezyk komunikatow*/
			if (canCommand == 4)
			{
				if (!global.canCommandData)
				{
					if(canByte1 <= 2) // LICZBA DOSTEPNYCH JEZYKOW -1
					{
						global.canCommandReady = canCommand;
						//if(global.canCommandData <= 2)
						global.canCommandData = canByte1;
					}
				}
			}
		}
	//}
//	else if (canDLC == 8) /*czas*/
//		{
//			uint8_t hoursHex = canByte2;
//			uint8_t minutesHex = canByte1;
//
//			uint8_t hoursTab[2][24] =
//			{{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23},
//					{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
//
//			uint8_t minTab[2][60] =
//			{{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
//					0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
//					0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,},
//					{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
//							30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}};
//
//			for (uint8_t i = 0; i < 24; i++) {if (hoursHex == hoursTab[0][i]) hours = hoursTab[1][i];}
//			for (uint8_t j = 0; j < 60; j++) {if (minutesHex == minTab[0][j]) minute = minTab[1][j];}
//		}
//	//}

	return 0;
}

uint32_t sendCanFrame (uint8_t workMode, uint8_t canDLC, uint8_t canByte0, uint8_t canByte1, uint8_t canByte2, uint8_t canByte3)
{
	uint8_t  dataToSend[4] = {0};
	uint16_t canId = 0;
	uint32_t canState = 0;
	uint32_t canError = HAL_CAN_GetError(&hcan);

	if (workMode == SEND_HEARTBEAT)
		canId = liftCanAdress.DiagTxAdress;
	else
		canId = liftCanAdress.TxAdress;

	pHeader.StdId = canId;
	pHeader.RTR = CAN_RTR_DATA;
	pHeader.IDE = CAN_ID_STD;
	pHeader.DLC = canDLC;

	dataToSend[0] = canByte0;
	dataToSend[1] = canByte1;
	dataToSend[2] = canByte2;
	dataToSend[3] = canByte3;

	canState = HAL_CAN_AddTxMessage(&hcan, &pHeader, dataToSend, &TxMailbox);

    if(canState != HAL_OK)
    {
       	uint8_t i = 0;

       	while(i < CAN_RETRANMISSION_ATTEMPTS)
    	{
    		canState = HAL_CAN_AddTxMessage(&hcan, &pHeader, dataToSend, &TxMailbox);

    		if(canState == HAL_OK)
    			break;
    	    else
    	    	i++;
    	}

       	return canState;
    }
    else
    {
    	return canState;
    }
}

void receiveCanFrame(void)
{
	uint8_t dataRx[4] = {0};

	HAL_CAN_GetRxMessage(&hcan,CAN_RX_FIFO0, &pRxHeader, (uint8_t*)dataRx);
	liftcanParser(pRxHeader.DLC, dataRx[0], dataRx[1], dataRx[2], dataRx[3]);
}

//void liftcanReset(uint8_t error)
//{
//	global.CanByteSeg1 = 40;
//	global.CanByteSeg2 = 3;
//	global.CanByteSeg3 = error;
//}
/* USER CODE END 1 */
