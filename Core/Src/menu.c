/*
 * menu.c
 *
 *  Created on: Oct 24, 2024
 *      Author: cezary.macik
 */

#include "menu.h"
#include "nodeParameter.h"
#include <stdio.h>

extern const char version[30];

uint8_t copyData;

Color colorRgbCode[COLOR_VALUE] = {
    {0, 	0, 		0	},	// BLACK
    {155, 	0, 		0	},  // RED
    {0, 	70, 	0	},  // GREEN
    {0, 	0, 		100	},  // BLUE
    {150, 	100, 	0	},  // YELLOW
    {128, 	128, 	128	},	// GRAY
    {128, 	0, 		128	},  // FIOLET
    {255,	0,		0	},	// RED
	{0,		255,	0	},  // GREEN
    {0,		0,		255	}   // BLUE
};

const char *menuLanguage[3] = {"HIDDEN","PL","EN"};
const char *menuOnOff[][3] =
{
		{"OFF","WYLACZONY","OFF"},
		{"ON","WLACZONY","ON"}
};
const char *menuParamState[3] = {"PARAMETERS SAVE!","PARAMETY ZAPISANE!", "PARAMETERS SAVE!"};

const char *menuSide[][3]=
{
	{"MAIN","GLOWNA","MAIN"},
	{"PASSANGE","PRZELOTOWA","PASSANGE"}
};

const char *menuDescriptionText[][3]=
{
/*0*/	{"FLOOR:","PIETRO:","FLOOR:"},
/*1*/	{"SIDE:","STRONA:","SIDE:"},
/*2*/	{"BRIGHTNESS:","JASNOSC:","BRIGHTNESS:"},
/*3*/	{"ROTATE:","ORIENTACJA:","ROTATE:"},
/*4*/	{"BUZZER:","BUZZER:","BUZZER:"},
/*5*/	{"GONG VOL:","GLOSNOSC GONGU:","GONG VOL:"},
/*6*/	{"EO|EU FLASH:","MIGANIE EO|EU:","EO|EU FLASH:"},
/*7*/	{"LANGUAGE:","JEZYK:","LANGUAGE:"},
/*8*/	{"COLOR:","KOLOR:","COLOR:"},
/*9*/	{"SAVE PARAMETERS","ZAPISZ PARAMETRY","SAVE PARAMETERS"},
/*10*/	{"EXIT","WYJSCIE","EXIT"},
};

void tftDispMenu (uint8_t status)
{
	uint8_t vertical = 0;

	if(nodeParameters[ROTATE].wartosc == 2 || nodeParameters[ROTATE].wartosc == 3)
		vertical = 1;

	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();

	EVE_CLEAR_COLOR_RGB(
		colorRgbCode[nodeParameters[COLOR].wartosc].r,
		colorRgbCode[nodeParameters[COLOR].wartosc].g,
		colorRgbCode[nodeParameters[COLOR].wartosc].b);

	EVE_CLEAR(1,1,1);
	EVE_COLOR_RGB(255,255,255);
	EVE_CMD_SETROTATE(nodeParameters[ROTATE].wartosc);
	EVE_CMD_SETFONT2(2,0,48);
	EVE_CMD_ROMFONT(1, 31);

	EVE_CMD_TEXT(3, menuPosSel, 18, 0,  	">");	EVE_CMD_TEXT(240, menuPosSel, 18, 0,  "<");

	EVE_CMD_TEXT(10, 0, 18, 0,  				version);
	EVE_CMD_TEXT(10, 20, 18, 0,  				"TEST TFT COLOR:");
	EVE_CMD_TEXT(10, _LIFTCAN_ADRESS, 18, 0,  	menuDescriptionText[0][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_NUMBER(160, _LIFTCAN_ADRESS,  18, 0, nodeParameters[LIFCAN_ADRESS].wartosc);
	EVE_CMD_TEXT(10, _SIDE, 18, 0,  			menuDescriptionText[1][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_TEXT(160, _SIDE,  18, 0,  		 	 menuSide[nodeParameters[SIDE].wartosc][nodeParameters[LANGUAGE].wartosc]);
	EVE_CMD_TEXT(10, _BRIGHTTNESS, 18, 0,  		menuDescriptionText[2][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_NUMBER(160, _BRIGHTTNESS,  18, 0,  	 nodeParameters[BRIGHTTNESS].wartosc);
	EVE_CMD_TEXT(10, _ROTATE, 18, 0, 			menuDescriptionText[3][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_NUMBER(160, _ROTATE, 18, 0, 		 nodeParameters[ROTATE].wartosc);
	EVE_CMD_TEXT(10, _BUZZER, 18, 0, 			menuDescriptionText[4][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_TEXT(160, _BUZZER, 18, 0, 		 	 menuOnOff[nodeParameters[BUZZER].wartosc][nodeParameters[LANGUAGE].wartosc]);
	EVE_CMD_TEXT(10, _GONG_VOL, 18, 0, 			menuDescriptionText[5][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_NUMBER(160, _GONG_VOL, 18, 0, 		 nodeParameters[GONG_VOL].wartosc);
	EVE_CMD_TEXT(10, _FLASHING_ARROWS, 18, 0, 	menuDescriptionText[6][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_TEXT(160, _FLASHING_ARROWS, 18, 0,   menuOnOff[nodeParameters[FLASHING_ARROWS].wartosc][nodeParameters[LANGUAGE].wartosc]);
	EVE_CMD_TEXT(10, _CODE, 18, 0, 				menuDescriptionText[7][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_TEXT(160, _CODE, 18, 0, menuLanguage[nodeParameters[LANGUAGE].wartosc]);
	EVE_CMD_TEXT(10, _COLOR, 18, 0, 			menuDescriptionText[8][nodeParameters[LANGUAGE].wartosc]);			EVE_CMD_NUMBER(160, _COLOR, 18, 0, 			 nodeParameters[COLOR].wartosc);
	//EVE_CMD_TEXT(10, COPY_DATA, 18, 0, 			"--------");							EVE_CMD_NUMBER(220, COPY_DATA, 18, 0, 		 nodeParameters[SD_FLASH].wartosc);
	//EVE_CMD_TEXT(10, _BACK_B, 18, 0, 			"--------");							EVE_CMD_NUMBER(220, _BACK_B, 18, 0, 		 nodeParameters[BACK_B].wartosc);
	EVE_CMD_TEXT(10, SAVE_PARAMETER, 18, 0, 	menuDescriptionText[9][nodeParameters[LANGUAGE].wartosc]);    		if(status == HAL_OK)
																													{if(vertical)
																															EVE_CMD_TEXT(10, 340, 18, 0, menuParamState[nodeParameters[LANGUAGE].wartosc]);
																														else
																															EVE_CMD_TEXT(10, 260, 18, 0, menuParamState[nodeParameters[LANGUAGE].wartosc]);}
	EVE_CMD_TEXT(10, EXIT, 18, 0, 				menuDescriptionText[10][nodeParameters[LANGUAGE].wartosc]);

	if(vertical)
	{
		char buf[30] = {0};
		snprintf(buf, sizeof(buf), "LIFTCAN Status R:%ld T:%ld", ((CAN->ESR >> CAN_ESR_REC_Pos) & 0xFF), ((CAN->ESR >> CAN_ESR_TEC_Pos) & 0xFF));
		EVE_CMD_TEXT(10, 260, 18, 0, buf);
		EVE_CMD_SETBASE(16);
		EVE_CMD_TEXT(10, 280, 18, 0, "LIFTCAN CODE(hex):");
		EVE_CMD_NUMBER(160, 280, 18, 0, CAN->ESR);

		if(CAN->ESR & CAN_ESR_EWGF_Msk) EVE_CMD_TEXT(10, 300, 18, 0, "LIFTCAN ERROR LEVER WARN!");
		if(CAN->ESR & CAN_ESR_BOFF_Msk) EVE_CMD_TEXT(10, 320, 18, 0, "LIFTCAN OFF!");

		EVE_CMD_SETBASE(10);
	}

	EVE_DISPLAY();
	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

void tftMatrixTest (void)
{
	for(uint16_t i = 0; i <= 255; i++)
	{
		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_CLEAR_COLOR_RGB(i,0,0);

		EVE_CLEAR(1,1,1);
		EVE_DISPLAY();
		EVE_DISPLAY();
		EVE_CMD_SWAP();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		HAL_Delay(1);
	}

	for(uint16_t i = 0; i <= 255; i++)
	{
		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_CLEAR_COLOR_RGB(0,i,0);

		EVE_CLEAR(1,1,1);
		EVE_DISPLAY();
		EVE_DISPLAY();
		EVE_CMD_SWAP();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		HAL_Delay(1);
	}

	for(uint16_t i = 0; i <= 255; i++)
	{
		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_CLEAR_COLOR_RGB(0,0, i);

		EVE_CLEAR(1,1,1);
		EVE_DISPLAY();
		EVE_DISPLAY();
		EVE_CMD_SWAP();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		HAL_Delay(1);
	}
}

