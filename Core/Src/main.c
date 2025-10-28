/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
//#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "buzzer.h"
#include "EVE.h"
#include "EVE_config.h"
#include "FT8xx.h"
#include "HAL.h"
#include "sendString.h"
#include "nodeParameter.h"
#include "menu.h"
#include "gvl.h"
#include "memoryOperation.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

const char version[40] = {"Lift Service S.A. MW-TFT v1.4"};

#define BLACK 0
#define FLASH_EU_EO_ARROW 1
#define FLASH_TIME_500MS 500
#define CHECK_TIME_100MS 100
#define MENU_TIME_20S	20000
#define ARROW_UP_EO 1
#define ARROW_DN_EU 2
#define EMPTY_EEPROM_CELL 255
#define MENU_STEP 20
#define ARROW_UP_GONG 	126
#define ARROW_BOTH_GONG 128
#define ARROW_DWN_GONG 	127
#define LIFTCAN_BRIGHTNESS_REGULATION 3
#define LIFTCAN_LANGUAGE_REGULATION 4
#define STM32F072 0x448
#define ARROW_UP 	126
#define ARROW_DWN 	127
#define ARROW_BOTH 	128
#define PW_KABINOWY 0xFF
#define LANG_HIDDEN 0
#define IN_PROGRESS 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
enum
{
	ARROW_UP_DIR = 1,
	ARROW_DOWN_DIR,
	ARROW_CENTER_DIR
} arrows;

enum
{
	BUZZER_OFF,
	BUZZER_ON
} buzzerState;

volatile struct {
	volatile bool btnPressOn[5];
	volatile bool btnPressLast;
	volatile uint8_t debounceMs;
} buttons;

volatile uint8_t menuScreen = false;
volatile uint8_t status = 5;
static volatile uint16_t hearthBeatTime;

volatile uint16_t menuPosSel = 40;
volatile uint16_t flashTime500ms = FLASH_TIME_500MS;
volatile uint16_t ms100CheckTime = CHECK_TIME_100MS;
volatile uint16_t sec5CheckTime;
volatile uint16_t timeToOffMenu;
volatile uint16_t flash;
volatile uint16_t sec;
uint16_t buzzer_delay, buzzer_count, buzzer_status, buzzer_start;

uint8_t playGong = true;
static volatile uint8_t gongType;
volatile uint8_t errorHandler;
volatile uint8_t callDir;
volatile uint8_t gongArrow;

extern const char *floorSpecDescriptions[][2];
extern volatile uint8_t ownSec, langtime;
volatile uint8_t  copytime;

extern volatile expresstravel;

uint16_t liftcanMode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern int dispStr(const char* str, int16_t y, uint8_t reset);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void checkButtonState(void);
void checkButtons(void);
void outOfMenu(void);

/*-------------------------PONIZEJ DZIALAJA FUNKCJE-----------------------------*/
void draw_arrow(int color_r, int color_g, int color_b, int alpha, int line_width, int x, int y, int arrow_height, int arrow_width, int arrow)
{
	#define OFFSET_Y 190

	#define ARROW_UP 	126
	#define ARROW_DWN 	127
	#define ARROW_BOTH 	128

	int16_t xL1, xL2 = 0;
	int16_t yL2 = 240;
	uint8_t landscape = 0;

    EVE_COLOR_RGB(color_r, color_g, color_b);
    EVE_LINE_WIDTH(line_width * 16);
    EVE_BEGIN(3);

    if(nodeParameters[ROTATE].wartosc == 0 || nodeParameters[ROTATE].wartosc == 1)
    {
    	x = 380;
		y = 20;
		xL1 = -250;
		xL2 = -50;
		yL2 = 0;
		landscape = 1;
    }

    if (arrow == ARROW_UP_DIR || arrow == ARROW_UP)     // Rysowanie strzałki w górę
    {
        EVE_VERTEX2F((x + arrow_width) * 16, (y + 200) * 16);
        EVE_VERTEX2F(x * 16, (y - arrow_height + 200) * 16);
        EVE_VERTEX2F(x * 16, (y - arrow_height + 200) * 16);
        EVE_VERTEX2F((x - arrow_width) * 16, (y + 200) * 16);
        EVE_VERTEX2F((x - arrow_width) * 16, (y + 200) * 16);
        EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        EVE_VERTEX2F((x + arrow_width) * 16, (y + 200) * 16);

        if(landscape && arrow == ARROW_UP)
        {
        	EVE_VERTEX2F((x + arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + 200) * 16);
        	EVE_VERTEX2F((x - arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x - arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        	EVE_VERTEX2F((x + arrow_width  + xL1) * 16, (y + 200) * 16);
        }

        if(arrow == ARROW_UP && !landscape)
        {
        	EVE_VERTEX2F((x + arrow_width) * 16, (y + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y - arrow_height + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y - arrow_height + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x - arrow_width) * 16, (y + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x - arrow_width) * 16, (y + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200 + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x + arrow_width) * 16, (y + 200 + OFFSET_Y) * 16);
        }
    }
    if (arrow == ARROW_DOWN_DIR || arrow == ARROW_DWN)     // Rysowanie strzałki w dół
    {
        EVE_VERTEX2F((x - arrow_width) * 16, y * 16);
        EVE_VERTEX2F(x * 16, (y + arrow_height) * 16);
        EVE_VERTEX2F(x * 16, (y + arrow_height) * 16);
        EVE_VERTEX2F((x + arrow_width) * 16, y * 16);
        EVE_VERTEX2F((x + arrow_width) * 16, y * 16);
        EVE_VERTEX2F(x * 16, (y + arrow_height - (arrow_height - 50)) * 16);
        EVE_VERTEX2F(x * 16, (y + arrow_height - (arrow_height - 50)) * 16);
        EVE_VERTEX2F((x - arrow_width) * 16, y * 16);

        if(landscape && arrow == ARROW_DWN)
         {
        	EVE_VERTEX2F((x - arrow_width + xL1) * 16, (y + yL2) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y + arrow_height + yL2) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y + arrow_height + yL2) * 16);
        	EVE_VERTEX2F((x + arrow_width + xL1) * 16, (y + yL2) * 16);
        	EVE_VERTEX2F((x + arrow_width + xL1) * 16, (y + yL2) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y + arrow_height - (arrow_height - 50) + yL2) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y + arrow_height - (arrow_height - 50) + yL2) * 16);
        	EVE_VERTEX2F((x - arrow_width + xL1) * 16, (y + yL2) * 16);
         }

        if(arrow == ARROW_DWN && !landscape)
        {
	       	EVE_VERTEX2F((x - arrow_width) * 16, (y + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y + arrow_height + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y + arrow_height + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x + arrow_width) * 16, (y + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x + arrow_width) * 16, (y + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y + arrow_height - (arrow_height - 50) + OFFSET_Y) * 16);
        	EVE_VERTEX2F(x * 16, (y + arrow_height - (arrow_height - 50) + OFFSET_Y) * 16);
        	EVE_VERTEX2F((x - arrow_width) * 16, (y + OFFSET_Y) * 16);
        }
    }
    else if (arrow == ARROW_CENTER_DIR || arrow == ARROW_BOTH)     // Rysowanie obu strzałek
    {
    	if(!landscape)
    	{
    		EVE_VERTEX2F((x + arrow_width) * 16, (y + 200) * 16);
    		EVE_VERTEX2F(x * 16, (y - arrow_height + 200) * 16);
    		EVE_VERTEX2F(x * 16, (y - arrow_height + 200) * 16);
    		EVE_VERTEX2F((x - arrow_width) * 16, (y + 200) * 16);
    		EVE_VERTEX2F((x - arrow_width) * 16, (y + 200) * 16);
    		EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
    		EVE_VERTEX2F(x * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
    		EVE_VERTEX2F((x + arrow_width) * 16, (y + 200) * 16);
    	}
    	else
        {
    		EVE_VERTEX2F((x + arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + 200) * 16);
        	EVE_VERTEX2F((x - arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x - arrow_width  + xL1) * 16, (y + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        	EVE_VERTEX2F((x + xL1) * 16, (y - arrow_height + (arrow_height - 50) + 200) * 16);
        	EVE_VERTEX2F((x + arrow_width  + xL1) * 16, (y + 200) * 16);
        }

        EVE_VERTEX2F((x - arrow_width + xL2) * 16, (y + yL2) * 16);
        EVE_VERTEX2F((x + xL2) * 16, (y + arrow_height + yL2) * 16);
        EVE_VERTEX2F((x + xL2) * 16, (y + arrow_height + yL2) * 16);
        EVE_VERTEX2F((x + arrow_width + xL2) * 16, (y + yL2) * 16);
        EVE_VERTEX2F((x + arrow_width + xL2) * 16, (y + yL2) * 16);
        EVE_VERTEX2F((x + xL2) * 16, (y + arrow_height - (arrow_height - 50) + yL2) * 16);
        EVE_VERTEX2F((x + xL2) * 16, (y + arrow_height - (arrow_height - 50) + yL2) * 16);
        EVE_VERTEX2F((x - arrow_width + xL2) * 16, (y + yL2) * 16);
    }

    EVE_END();
}

//void eveMsgNum(int16_t posX, int16_t posY, uint16_t value)
//{
//	EVE_LIB_BeginCoProList();
//	EVE_CMD_DLSTART();
//	EVE_CLEAR_COLOR_RGB(0,0,0);
//	EVE_CLEAR(1,1,1);
//	EVE_CMD_SETROTATE(2);
//	EVE_CMD_ROMFONT(1, 31);
//	EVE_CMD_NUMBER(posX, posY, 18, 0, value);
//	EVE_DISPLAY();
//	EVE_DISPLAY();
//	EVE_CMD_SWAP();
//	EVE_LIB_EndCoProList();
//	EVE_LIB_AwaitCoProEmpty();
//}

void eveMsgXY(int16_t posX, int16_t posY, int16_t font, char *str)
{
	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();
	EVE_CLEAR_COLOR_RGB(0,0,0);
	EVE_CLEAR(1,1,1);
	EVE_CMD_SETROTATE(2);
	EVE_CMD_ROMFONT(1, 31);
	EVE_CMD_TEXT(posX, posY, font, 0, (char *)str);
	EVE_DISPLAY();
	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

void eveMsgMultiLine(int16_t startX, int16_t startY, int16_t font, int16_t lineSpacing, int16_t lineCount, ...)
{
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
    EVE_CMD_SETROTATE(2);
    EVE_CMD_ROMFONT(1, 31);

    va_list args;
    va_start(args, lineCount);

    for (int16_t i = 0; i < lineCount; i++)
    {
        char *line = va_arg(args, char *);

        if (line != NULL)
        {
            EVE_CMD_TEXT(startX, startY + i * lineSpacing, font, 0, line);
        }
    }

    va_end(args);

    EVE_DISPLAY();
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
}

void tftDisp3 (uint8_t floorCode, uint8_t displayArrow, uint8_t arrowDir)
{
	#define	ARROW_THICK 4
	#define ARROW_HEIGHT 200
	#define ARROW_WIDTH  80

	int16_t y = 430;

	uint8_t resetScrollSpecialFloor = false;
	static uint8_t scrollSpecialFloor = false;

	if(nodeParameters[ROTATE].wartosc == 0 || nodeParameters[ROTATE].wartosc == 1)
		y = 230;

	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();

	EVE_CLEAR_COLOR_RGB(
		colorRgbCode[nodeParameters[COLOR].wartosc].r,
		colorRgbCode[nodeParameters[COLOR].wartosc].g,
		colorRgbCode[nodeParameters[COLOR].wartosc].b);

	EVE_CLEAR(1,1,1);
	EVE_COLOR_RGB(255,255,255);
	EVE_CMD_SETROTATE(nodeParameters[ROTATE].wartosc);

	if(arrowDir < ARROW_CENTER_DIR)
	{
		if(displayArrow % 2)	//draw_arrow(255, 255, 255, alpha, 15, 136, 250, 150, 50, arrowDir);
			draw_arrow(255, 255, 255, 255, ARROW_THICK, 136, 220, ARROW_HEIGHT, ARROW_WIDTH, arrowDir);
		/*STRZALKA KIERUNKU JAZDY, WYNIKA Z LS2020, GDY SEG2 != 0*/
	}

	if(floorCode == ARROW_UP || floorCode == ARROW_DWN || floorCode == ARROW_BOTH) /*TODO maja sie roznic od strzalek jazdy*/
	{
		if(displayArrow % 2)
			draw_arrow(255, 255, 255, 255, ARROW_THICK, 136, 20, ARROW_HEIGHT, ARROW_WIDTH, floorCode);

		if(nodeParameters[LANGUAGE].wartosc != LANG_HIDDEN)
		{
		gongType = floorCode;

		if(!scrollSpecialFloor) resetScrollSpecialFloor = true;

			switch(floorCode)
			{
				case ARROW_UP:
					//scrollSpecialFloor = dispStr(floorSpecDescriptions[12][nodeParameters[LANGUAGE].wartosc], y, resetScrollSpecialFloor);
					break;
				case ARROW_DWN:
					//scrollSpecialFloor = dispStr(floorSpecDescriptions[13][nodeParameters[LANGUAGE].wartosc], y, resetScrollSpecialFloor);
					break;
				case ARROW_BOTH:
					//scrollSpecialFloor = dispStr(floorSpecDescriptions[14][nodeParameters[LANGUAGE].wartosc], y, resetScrollSpecialFloor);
					break;
			}
		}
		/*STRZALKA DOJAZDU NA PRZYSTANEK, KOD LS2020 SEG == 126, 127 LUB 128 DLATEGO RYSOWANE SA NA GORZE*/

	}
	else
	{
		gongType = 0;
		/*
			canByteSeg3 = moze to dziwne, ale w can.c jest taka funkcja.
			if ((global.CanByteSeg1 < FLOOR_CODE) && (global.CanByteSeg2 < DETECT_LS2020_FAIL))
				global.CanByteSeg3 = global.CanByteSeg1;
			Dzieki temu mamy zapamietany ostatni kod pietra
	 	*/
		if(nodeParameters[GONG_VOL].wartosc > 0)
			playGong = 1;

		decodeCodeFloor(&global.CanByteSeg3, &displayArrow, &global.CanByteSeg2, nodeParameters[LANGUAGE].wartosc, floorCode);
	}

	EVE_DISPLAY();
	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

void dataLoadFlash(void)
{
	MX_DMA_Init();
	MX_SPI2_Init();
	__init_storage(); /*Init LittleFs*/
	eveMsgMultiLine(10, 70, 18, 1, 1,(char *)">File SYSTEM MOUNTED!");
}

void dataCopyFn(void)
{
	  if(buttons.btnPressOn[UP_BUTTON]) dataCopy = true;

	  if(dataCopy)
	  {
		  MX_SPI2_Init();
		  MX_FATFS_Init();
		  __init_storage();

		  FatFsResult = f_mount(&SdFatFs, "0:", 0);

		  if(FatFsResult == FR_OK)
		  {
			  eveMsgMultiLine(10, 70, 18, 20, 3,
					  "Copy data SD to FLASH mode",
					  ">Please put a SD card to slot<",
					  "and press ENT to start...",
					  "Or RESET to abort" );


			  while(!buttons.btnPressOn[ENT_BUTTON]){
	                if (copytime <= 0) return; // wyjście po bezczynności
	            }

			  if(
				 copyFileSdToFlash("0:font.raw", 	"font_ff.raw") 		== FR_OK &&
				 copyFileSdToFlash("0:font_pl.raw", "font_ff_pl.raw")	== FR_OK &&
				 copyFileSdToFlash("0:numbers.raw", "font_num_pl.raw")	== FR_OK
			  )
			  {
				  eveMsgMultiLine(10, 70, 18, 20, 3,
						  "Copy SD->FLASH OK END!",
						  "Remove a SD Card from slot and",
						  "unplug&plug power supply!" );
				  while(1);
			  }
			  else
			  {
				  eveMsgMultiLine(10, 70, 18, 20, 3,
						  "Copy SD->FLASH Error!",
						  "Check SD card and retry again!",
						  "Please unplug&plug power supply!");
				  while(1);
			  }
		  }
//		  else
//		  {
//			  eveMsgMultiLine(10, 70, 18, 20, 3,
//			  				"Copy SD->FLASH Error!",
//			  			    "SD Card not detect!",
//			  				"Please unplug&plug power supply!");
//			  while(1);
//		  }
	  }
	  else
	  {
		  MX_DMA_Init();
		  MX_SPI2_Init();
		  __init_storage(); /*Init LittleFs*/
		 eveMsgMultiLine(10, 70, 18, 1, 1,(char *)">SYSTEM PLIKOW ZAMONTOWANY!");
	  }

	  if(buttons.btnPressOn[DN_BUTTON])
	  {
		  eveMsgMultiLine(10, 70, 18, 20, 4,
		 				 "Erase Flash",
		 				 "Press LEFT, RIGHT and ENT",
		 				 "to start...",
						 "Or RESET to abort" );

		  while(!buttons.btnPressOn[LEFT_BUTTON]);
		  while(!buttons.btnPressOn[RIGHT_BUTTON]);
		  while(!buttons.btnPressOn[ENT_BUTTON]);

		  if(lfs_format(&Lfs, &LfsConfig) == FR_OK)
			  eveMsgMultiLine(10, 70, 18, 20, 2,
		  				 "Erase Flash END!",
		  				 "Please unplug&plug power supply!");
		  else
			  eveMsgMultiLine(10, 70, 18, 20, 2,
					  	 "Erase Flash Error!",
			  		  	 "Please unplug&plug power supply!");
		  while(1);
	  }
}

uint8_t playGongFn(uint8_t step)
{
	#define MUTE 0
	#define AMP_OFF 0

	if(nodeParameters[GONG_VOL].wartosc == MUTE)
	{
		HAL_MemWrite8(EVE_REG_GPIOX, AMP_OFF);
		HAL_MemWrite8 (EVE_REG_VOL_SOUND,	nodeParameters[GONG_VOL].wartosc); //set the volume to maximum
	}
	else
	{
		HAL_MemWrite8 (EVE_REG_VOL_SOUND,	nodeParameters[GONG_VOL].wartosc * 25);
	}

	if(playGong)
	{
		switch(gongType)
		{
			case ARROW_UP_GONG:
				HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
				HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
				HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
				HAL_MemWrite8(EVE_REG_PLAY, 1);

				while(HAL_MemRead8(EVE_REG_PLAY));

				playGong = gongType = false;
				HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
				break;
			case ARROW_DWN_GONG:

				if(!step)
				{
					HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
					HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
					HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					if(!HAL_MemRead8(EVE_REG_PLAY))
						step = 1;
				}
				else if(step == 1)
				{
					HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					step = 0;
					playGong = gongType = false;
					HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
				}
				break;
			case ARROW_BOTH_GONG:
				if(!step)
				{
					HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
					HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
					HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					if(!HAL_MemRead8(EVE_REG_PLAY))
						step = 1;
				}
				else if(step == 1)
				{
					HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					if(!HAL_MemRead8(EVE_REG_PLAY))
						step = 2;
				}
				else if(step == 2)
				{
					HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					step = 0;
					playGong = gongType = false;
					HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
				}
				break;
		}
	}

	return step;
}

void playCallBuzzerFn(uint8_t _callDir)
{
	//if(playGong)
	{
		switch(_callDir)
		{
			case 1:
				if(!sec5CheckTime)
				{
					sec5CheckTime = 2500;
					HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
					HAL_MemWrite8 (EVE_REG_VOL_SOUND, MAX_VOL);
					HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
					HAL_MemWrite16(EVE_REG_SOUND, BUZZER_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					playGong = _callDir = false;
					HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
				}
				break;
			case 2:
				if(!sec5CheckTime)
				{
					sec5CheckTime = 2500;
					HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
					HAL_MemWrite8 (EVE_REG_VOL_SOUND, MAX_VOL);
					HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
					HAL_MemWrite16(EVE_REG_SOUND, BUZZER_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
					HAL_MemWrite8 (EVE_REG_VOL_SOUND, MAX_VOL);
					HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
					HAL_MemWrite16(EVE_REG_SOUND, BUZZER_SOUND);
					HAL_MemWrite8(EVE_REG_PLAY, 1);

					while(HAL_MemRead8(EVE_REG_PLAY));

					playGong = _callDir = false;
					HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
				}
				break;
		}
	}
}

/*NIE WOLNO USUWAC! JEZELI MA DZIALAC UPLOAD FIRMWARE Z KARTY SD*/
#define VECTOR_TABLE_SIZE 48
#define APP_VECTOR_TABLE  ((uint32_t*)0x08008000)
#define RAM_VECTOR_TABLE  ((uint32_t*)0x20000000)

void relocate_vector_table_to_ram(void)
{
	for (uint32_t i = 0; i < VECTOR_TABLE_SIZE; i++)
	{
		RAM_VECTOR_TABLE[i] = APP_VECTOR_TABLE[i];
	}
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
}
/*NIE WOLNO USUWAC! END*/

/* USER CODE END 0 */
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	/*NIE WOLNO USUWAC JEZELI CHCEMY ABY DZIALAL NAM UPLOAD FIRMWARE Z KARTY SD!
	 * TE FUNKCJE MUSZA BYC NA SAMYM POCZATKU*/
	//__enable_irq();
	//relocate_vector_table_to_ram();
	/*NIE WOLNO USUWAC JEZELI CHCEMY ABY DZIALAL NAM UPLOAD FIRMWARE Z KARTY SD! (zakomentowac jesli DEBUG)*/

	//HAL_DeInit();

	uint32_t arrows = false;
	uint32_t arrowTmp = 0;
	uint32_t firstRun = true;
	uint32_t stepGong = 0;
	uint32_t displayShutdown = false;
	uint32_t prevDisplayOff = false;
	uint32_t crcRes;
    uint32_t crcResFromEEPROM;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /*Te funkcje sa specjalne zakomentowane, poniewaz sa wywolywane w zaleznosci od kontekstu*/
  //MX_DMA_Init();
  //MX_SPI2_Init();
  //MX_FATFS_Init();
  /*END*/

  MX_SPI1_Init();
  //MX_USART1_UART_Init();
  MX_CAN_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_CRC_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);

  EVE_Init();

/**------------------**/
  eveMsgMultiLine(10, 10, 18, 20, 3,
		  version,
		  ">Inicjalized MW-TFT",
  	  	  ">Seek mermory");

  HAL_Delay(500); //bylo 500
  copytime = 4;
  dataCopyFn();
  //dataLoadFlash();

  eveMsgMultiLine(10, 10, 18, 20, 3,
  		  version,
  		  ">Inicjalized MW-TFT",
    	  ">FLASH RDY");

  HAL_Delay(500);

  crcRes = readParameters();
  crcResFromEEPROM = readCRCEEPROM();

  if (crcRes == crcResFromEEPROM)
  {
	  char buf[30] = {0};
	  snprintf(buf, sizeof(buf), "%lu == %lu", crcRes, crcResFromEEPROM);
	  eveMsgMultiLine(10, 10, 18, 20, 2,(char *)">EEPROM OK & CRC OK!", buf);
  }
  else
  {
	  char buf[30] = {0};
	  snprintf(buf, sizeof(buf), "%lu != %lu", crcRes, crcResFromEEPROM);
	  eveMsgMultiLine(10, 10, 18, 20, 3,(char *)">EEPROM CRC ERROR!", buf,"CHECK PARAMETERS!");
  }

  HAL_Delay(500);

  if( (loadAssetFromFlash(1, "font_ff.raw", 		RAM_G_LARGE_FONT_OFFSET) == 1) &&
	  (loadAssetFromFlash(2, "font_num_pl.raw", 	RAM_G_LARGE_NUMBER_OFFSET) == 1) &&
	  (loadAssetFromFlash(3, "font_ff_pl.raw", 		RAM_G_PL_FONT_OFFSET) == 1)
	 )

	  eveMsgMultiLine(10, 120, 18, 20, 1,(char *)">FONT FILE LOADED!");
  else
  {
	  eveMsgMultiLine(10, 120, 18, 20, 2,(char *)">FONT FILE NOT DETECT!", "RESTART MW-TFT");
	  HAL_Delay(5000);
      HAL_NVIC_SystemReset();
  }


  HAL_Delay(250);

  if(nodeParameters[LIFCAN_ADRESS].wartosc == 0)
  {
	  liftCanAdress.TxAdress = PW_KABINOWY;
	  liftcanMode = PW_KABINOWY;
  }
  else
	  liftCanAdress.TxAdress = nodeParameters[LIFCAN_ADRESS].wartosc-1;

  if (nodeParameters[SIDE].wartosc)
	  liftCanAdress.TxAdress |= 0x20;

  updateParamValue();
  //HAL_Delay(100 + ((liftCanAdress.TxAdress & 0x1F) << 2)); //jakis randomowy delay

  setupCANFilter(liftCanAdress.TxAdress);

  HAL_Delay(100 + ((liftCanAdress.TxAdress & 0x1F) << 2));

  hearthBeatTime = HEARTHBEAT_CAN;
  buttons.debounceMs = DEBOUNCE_MS;

	#if IWDG_ON
  	  MX_IWDG_Init();
  	  HAL_IWDG_Init(&hiwdg);
	#endif

  	sendCanFrame(DIAGNOSTIC_DATA, ONE_BYTE, 0x86, 0, 0, 0);

  //HAL_Delay(100);

  if((CAN->ESR >> 16) & 0xFF) /*DETEKCJA KOMUNIKACJI LIFTCAN PRZEZ LICZNIK BLEDOW TEC*/
	  global.CanByteSeg1 = BRAK_LIFTCAN /*KOD_BRAK_LIFTCAN*/;
  else
	  global.CanByteSeg1 = 100 /*KOD_DZWIG_NIECZYNNY*/;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  arrowTmp = arrows;

	  if(!hearthBeatTime)
	  {
		  if(liftcanMode != PW_KABINOWY)
		  	  sendCanFrame(LIFTCAN_DATA, ONE_BYTE, CheckedInputs, 0, 0, 0);
		  else
			  sendCanFrame(LIFTCAN_DATA, ONE_BYTE, 0x86, 0, 0, 0);

		  hearthBeatTime = HEARTHBEAT_CAN;
	  }

	  if(global.DataToSendCANReady)
	  {
		  global.DataToSendCANReady = false;

		  if(liftcanMode != PW_KABINOWY)
			  sendCanFrame(LIFTCAN_DATA, ONE_BYTE, CheckedInputs, 0, 0, 0);
		  else
			  sendCanFrame(LIFTCAN_DATA, ONE_BYTE, 0x86, 0, 0, 0);
	  }

	  if(global.canCommandReady == LIFTCAN_BRIGHTNESS_REGULATION)
	  {
		  updateParamValue();
		  global.canCommandReady = global.canCommandData = 0;
	  }

	  if(global.canCommandReady == LIFTCAN_LANGUAGE_REGULATION)
	  {
		  updateParamValue();
		  global.canCommandReady = global.canCommandData = 0;
	  }

	  if(nodeParameters[FLASHING_ARROWS].wartosc == FLASH_EU_EO_ARROW && liftcanMode != PW_KABINOWY)
	  {
		  arrows = false;

		  if(global.CanByteSeg2 == ARROW_UP_EO && flash) /*EO*/
		  {
			  if(global.flashEoEu) arrows |= 0x04;
			  else arrows |= 0x08;
		  }

		  if(global.CanByteSeg2 == ARROW_DN_EU && flash) /*EU*/
		  {
			  if(global.flashEoEu) arrows |= 0x20;
		  	  else arrows |= 0x10;
		  }

		  if(arrows != arrowTmp)
			  global.OutputReady = true;
	  }

	  if (nodeParameters[BUZZER].wartosc)
	  {
	  	 if ((CheckedInputs & 0x01) && (global.SetOutpus & 0x01)) playCallBuzzerFn(1);
	  	 if ((CheckedInputs & 0x02) && (global.SetOutpus & 0x02)) playCallBuzzerFn(2);
	  }

	  if(global.OutputReady)
	  {
		  arrowTmp = false;
		  setOutputs(global.SetOutpus | arrows);
		  global.OutputReady = false;
	  }

	  if (global.buzzerRunState) setBuzzer();

	  if(!menuScreen && !buzzer_status && expresstravel != IN_PROGRESS)
		  tftDisp3(global.CanByteSeg1, sec, global.CanByteSeg2);
	  else
		  checkButtonState();

	  stepGong = playGongFn(stepGong);

	  //if((firstRun < 10) && global.CanByteSeg2 != 100) /*dodatkowe wysylanie ramki statusu*/
	  //{
		  //sendCanFrame(DIAGNOSTIC_DATA, ONE_BYTE, 0x86, 0, 0, 0);
		  //HAL_Delay(1);
		  //firstRun++;

	  //}

	  /*if((firstRun == true) && global.CanByteSeg2 != 100)
	  {
		  HAL_Delay(100 + ((liftCanAdress.TxAdress & 0x1F) << 2));
		  sendCanFrame(DIAGNOSTIC_DATA, ONE_BYTE, 0x86, 0, 0, 0);
		  firstRun=false;
	  }*/

	  if(global.displayOff == true && displayShutdown == false)
	  {
		  HAL_MemWrite8(EVE_REG_PWM_DUTY, (1 * 28));
		  displayShutdown = true;
	  }

	  if(global.displayOff == false && prevDisplayOff == true)
	  {
		  displayShutdown = false;
		  HAL_MemWrite8(EVE_REG_PWM_DUTY, (nodeParameters[BRIGHTTNESS].wartosc * 28));
	  }

	  prevDisplayOff = global.displayOff;

	#if IWDG_ON
		HAL_IWDG_Refresh(&hiwdg);
	#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void checkButtonState(void)
{
	if(menuScreen)
	{
		for (uint8_t i = 0; i < 5; i++)
		{
			if (buttons.btnPressOn[i])
			{
				timeToOffMenu = MENU_TIME_20S;

				switch(i)
				{
	              case UP_BUTTON:
	            	  if(menuPosSel <= DEF_PAR) menuPosSel = EXIT;
	            	  else menuPosSel -= MENU_STEP;
	                  break;
	              case DN_BUTTON:
	            	  if(menuPosSel >= EXIT) menuPosSel = DEF_PAR;
	            	  else menuPosSel += MENU_STEP;
	                  break;
	              case LEFT_BUTTON:
	            	  if(nodeParameters[MENU_POSITION].wartosc <= nodeParameters[MENU_POSITION].min)
	            		  nodeParameters[MENU_POSITION].wartosc = nodeParameters[MENU_POSITION].min;
	            	  else
	            	  {
	            		  nodeParameters[MENU_POSITION].wartosc--;
	            		  if(nodeParameters[GONG_VOL].wartosc && menuPosSel == _GONG_VOL) checkGong();
	            	  }
	                  break;
	              case RIGHT_BUTTON:
	            	  if(nodeParameters[MENU_POSITION].wartosc >= nodeParameters[MENU_POSITION].max)
	            	  {
	            		  nodeParameters[MENU_POSITION].wartosc = nodeParameters[MENU_POSITION].max;
	            	  }
	            	  else
	            	  {
	            		  nodeParameters[MENU_POSITION].wartosc++;
	            		  if (nodeParameters[BUZZER].wartosc && menuPosSel == _BUZZER) checkBuzzerSound();
	            		  if (nodeParameters[GONG_VOL].wartosc && menuPosSel == _GONG_VOL) checkGong();
	            	  }
	                  break;
	              case ENT_BUTTON:
	            	  if(menuPosSel == SAVE_PARAMETER)
	            	  {
	            		  status = saveParameters();
	            		  tftDispMenu(status);
	            		  HAL_Delay(1000);

	            		  if(status == HAL_OK)
	            			  NVIC_SystemReset();
	            	  }
	            	  else if(menuPosSel == 20)
					  {
	            		  tftMatrixTest();
					  }
	            	  else if(menuPosSel == EXIT)
	            	  {
	            		  outOfMenu();
	            	  }
	                  break;
				}

				if(!buzzer_status)
				{
					tftDispMenu(status);
					updateParamValue();
				}

	         	buttons.btnPressOn[i] = false;  // Zresetowanie stanu po obsłudze
			}
		}

		if(timeToOffMenu == 0)
		{
			outOfMenu();
		}

		if(!ms100CheckTime && !buzzer_status) /*automatycznie odswiez co 100ms*/
			tftDispMenu(status);
	}
}

void outOfMenu(void)
{
	tftDisp3(0,0,0);
	menuPosSel = 40;
	menuScreen = false;
	status = 5;
}

void updateParamValue(void)
{
	/*ZDALNA ZMIANA PARAMETROW PRZEZ LIFTCAN*/
	if(nodeParameters[BRIGHTTNESS].wartosc > nodeParameters[BRIGHTTNESS].max || nodeParameters[BRIGHTTNESS].wartosc == EMPTY_EEPROM_CELL ||
	!nodeParameters[BRIGHTTNESS].wartosc)
	{
		nodeParameters[BRIGHTTNESS].wartosc = nodeParameters[BRIGHTTNESS].max;
		saveParameters();
	}

	if(global.canCommandData > 0 && global.canCommandReady == LIFTCAN_BRIGHTNESS_REGULATION)
	{
		nodeParameters[BRIGHTTNESS].wartosc = global.canCommandData;
		saveParameters();
	}

	if(global.canCommandReady == LIFTCAN_LANGUAGE_REGULATION)
	{
		nodeParameters[LANGUAGE].wartosc = global.canCommandData;
		saveParameters();
	}

	/*ZDALNA ZMIANA PARAMETROW PRZEZ LIFTCAN END*/

	HAL_MemWrite8(EVE_REG_PWM_DUTY, (nodeParameters[BRIGHTTNESS].wartosc * 28));

	if(nodeParameters[COLOR].wartosc == EMPTY_EEPROM_CELL)// && nodeParameters[BACK_G].wartosc == EMPTY_EEPROM_CELL && nodeParameters[BACK_B].wartosc == EMPTY_EEPROM_CELL)
		nodeParameters[COLOR].wartosc = BLACK; // nodeParameters[BACK_G].wartosc = nodeParameters[BACK_B].wartosc = BLACK;
}

void checkButtons(void)
{
	if((!HAL_GPIO_ReadPin(DN_S2_BTT_GPIO_Port, DN_S2_BTT_Pin))&&
			(!HAL_GPIO_ReadPin(UP_S1_BTT_GPIO_Port, UP_S1_BTT_Pin)))
	{
		buttons.debounceMs = DEBOUNCE_MS; buttons.btnPressLast = false;
	}
	else
	{

	if(!HAL_GPIO_ReadPin(UP_S1_BTT_GPIO_Port, UP_S1_BTT_Pin))
	{
		if(buttons.debounceMs > 0) buttons.debounceMs--;
		if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[UP_BUTTON] = buttons.btnPressLast = true;}
	}
	else if(!HAL_GPIO_ReadPin(DN_S2_BTT_GPIO_Port, DN_S2_BTT_Pin))
	{
		if(buttons.debounceMs > 0) buttons.debounceMs--;
		if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[DN_BUTTON] = buttons.btnPressLast = true;}
	}
	else if(!HAL_GPIO_ReadPin(LEFT_S3_BTT_GPIO_Port, LEFT_S3_BTT_Pin))
	{
		if(buttons.debounceMs > 0) buttons.debounceMs--;
		//if(menuPosSel == _GONG_VOL) {if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[LEFT_BUTTON] = true;}}
		/*else*/ if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[LEFT_BUTTON] = buttons.btnPressLast = true;}
	}
	else if(!HAL_GPIO_ReadPin(RIGHT_S4_BTT_GPIO_Port, RIGHT_S4_BTT_Pin))
	{
		if(buttons.debounceMs > 0) buttons.debounceMs--;
		//if(menuPosSel == _GONG_VOL) {if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[RIGHT_BUTTON] = true;}}
		/*else*/ if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[RIGHT_BUTTON] = buttons.btnPressLast = true;}
	}
	else if(!HAL_GPIO_ReadPin(ENT_S5_BTT_GPIO_Port, ENT_S5_BTT_Pin))
	{
		if(buttons.debounceMs > 0) buttons.debounceMs--;
		if(!buttons.debounceMs && !buttons.btnPressLast){buttons.btnPressOn[ENT_BUTTON] = buttons.btnPressLast = true; menuScreen = true;}
	}
	else
	{
		buttons.debounceMs = DEBOUNCE_MS; buttons.btnPressLast = false;
	}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM2) /*timer 1ms*/
	{
		if(hearthBeatTime > 0)
			hearthBeatTime--;

		if(flashTime500ms > 0)
			flashTime500ms--;

		if(ms100CheckTime > 0)
			ms100CheckTime--;

		if(!ms100CheckTime)
			ms100CheckTime = CHECK_TIME_100MS;
		else
			ms100CheckTime--;

		if(sec5CheckTime > 0)
			sec5CheckTime--;

		if(timeToOffMenu > 0)
			timeToOffMenu--;

		if (flashTime500ms == 0)
		{
			flashTime500ms = FLASH_TIME_500MS;

			if (flash == 0)
				flash = 1;
			else
				flash = 0;
		}
		else
			flashTime500ms--;

		checkInputs();
		checkButtons();
	}

	if(htim->Instance == TIM6) /*timer 1s*/
	{
		if(sec < 1)
			sec++;
		else
			sec = 0;

		if(ownSec > 0)
			ownSec--;

		if(langtime > 0)
			langtime--;

		if(copytime > 0)
			copytime--;

	}

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	receiveCanFrame();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  while (1)
  {
	  errorHandler = true;
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
