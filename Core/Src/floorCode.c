/*
 * floorCode.c
 *
 *  Created on: Nov 27, 2024
 *      Author: cezary.macik
 */

#include "main.h"
#include <stdbool.h>
#include "EVE.h"
#include "GVL.h"
#include "languages.h"
#include "nodeParameter.h"
#include "string.h"

//#pragma GCC poison floorSymbols

enum
{
	BUZZER_OFF,
	BUZZER_ON
} buzzerState2;

#define MAX_STR_LEN_HORI 13
#define ERROR_DETECT 3

void displayInError(uint8_t code,int16_t, uint8_t landscape, uint8_t lang);
int displayInExpressMode(uint8_t code, int16_t y, uint8_t landscape, uint8_t reset, uint8_t lang);
void decodeCodeFloor(uint8_t* canFloorCode, uint8_t* flashing, uint8_t *errorDetect, uint8_t lang, uint8_t lastCode);
void drawBatterySymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t chargeLevel);
void DrawFlameSymbol(int x, int y, int size);
void drawStairsSymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t stepCount);
void drawNoEntrySymbol(uint16_t x, uint16_t y, uint16_t diameter);
int outOforder(uint8_t lastCode, int16_t yL1, int16_t yL2, uint8_t landscape, uint8_t lang);
//void drawPlusSymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t borderThickness);
void DrawTriangleWithExclamation(int x, int y, int size);
int scrollText (const char *text, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t speed, uint16_t gap, uint8_t reset);
int dispStr(const char* str, int16_t y, uint8_t reset);
void drawSimpleArrowUp(int line_width, int x, int y, int arrow_height, int arrow_width);
void drawArrowSymbol(uint16_t x, uint16_t y, uint16_t length, uint16_t height);


volatile uint8_t scrollSpecialFloor;
volatile uint8_t scrollFloorNames;
volatile uint8_t scroolErrorNames;
volatile uint8_t scroolExtraNames;
volatile uint8_t lastFloorCode;
volatile uint8_t previousCase  = -1;
volatile uint8_t nameMode;
extern uint8_t playGong;
extern volatile uint16_t sec;
volatile uint8_t ownSec, langtime;
enum{
	LANG_HIDDEN,
	PL,
	EN,
//	LT,
//	SK,
//	IT,
//	FR,
//	RU
} lang;

enum {
	FLOOR_NAME = 1,
	SPECIAL_FLOOR,
	EXTRA_NAME,
	ERROR_NAME,
	LARGE_FLOOR_NUMBER
} floorNameMode;

enum{
	KOD_PIETRO_0,
	KOD_PIETRO_32 = 32,
	KOD_PIETRO_0B = 33,
	KOD_PIETRO_6B = 39,
	KOD_PIETRO_0A = 40,
	KOD_PIETRO_9A = 49,
	KOD_PIETRO_MIN_1_PRIM = 50,
	KOD_PIETRO_1_PRIM = 51,
	KOD_PIETRO_0_PRIM = 60,
	KOD_POZIOM_A = 61,
	KOD_POZIOM_J = 70,
	KOD_PIETRO_01 = 71,
	KOD_PIETRO_00 = 80,
	KOD_PIETRO_7B = 81,
	KOD_PIETRO_8B,
	KOD_PIETRO_9B,
	KOD_SUTERENA_3,
	KOD_BALKON_3,
	KOD_PARKING_3,
	KOD_PARKING_TECHNICZNY,
	KOD_GARAZ_1,
	KOD_GARAZ_2,
	KOD_POZIOM_SPECJALNY,
	KOD_SZATNIA,
	KOD_GARAZ,
	KOD_AMIFITEATR,
	KOD_BALKON_1,
	KOD_BALKON_2,
	KOD_BALKON,
	KOD_PARKING_1,
	KOD_PARKING_2,
	KOD_PARTER_WYSOKI,
	KOD_DZWIG_NIECZYNNY,
	KOD_PIETRO_MINUS_1 = 101,
	KOD_PIETRO_MINUS_9 = 109,
	KOD_JAZDA_ORIENTUJACA,
	KOD_PARTER_NISKI,
	KOD_PRZECIAZENIE,
	KOD_JAZDA_SPECJALNA,
	KOD_HOL,
	KOD_SUTERENENA,
	KOD_SUTERENA_1,
	KOD_SUTERENA_2,
	KOD_ANTRESOLA,
	KOD_ANTRESOLA_1,
	KOD_ANTRESOLA_2,
	KOD_RECEPCJA,
	KOD_PARTER,
	KOD_PARTER_1,
	KOD_PARTER_2,
	KOD_PARKING,
	KOD_KIERUNEK_DALSZA_JAZDA_GORA,
	KOD_KIERUNEK_DALSZA_JAZDA_DOL,
	KOD_KIERUNEK_DALSZA_JAZDA_GORA_DOL,
	KOD_JAZDA_KONTROLNA = 129,
	KOD_ZJAZD_AWARYJNY,
	KOD_REMONT_WINDY,
	KOD_JAZDA_POZAROWA,
	KOD_BRAK_LIFTCAN = 200
}floorCode;

enum{
	POZIOM,
	PIETRO,
	PARTER,
	SUTERENA,
	BALKON,
	PARKING,
	GARAZ,
	SZATNIA,
	AMFITEATR,
	HOL,
	ANTRESOLA,
	RECEPCJA,
	WYSOKI,
	NISKI,
	TECHNICZNY,
	SPECJALNY,
	PRZECIAZENIE,
	WINDA,
	NIECZYNNA,
	NAPRAWA,
	WINDY,
	ZJAZD,
	AWARYJNY,
	JAZDA,
	POZAROWA,
	ORIENTUJACA,
	SPECJALNA,
	DZWIG,
	REMONT,
	LIFTCAN_NIE_WYKRYTY
} floorName;

int showFloorOrLevel(uint8_t code, int16_t y, uint8_t reset, uint8_t lang)
{
	uint8_t status = 0;

	if((code > KOD_PIETRO_0 && code <= KOD_PIETRO_0_PRIM) ||
	   (code >= KOD_PIETRO_01 && code <= KOD_PIETRO_9B) ||
	   (code >= KOD_PIETRO_MINUS_1 && code <= KOD_PIETRO_MINUS_9) )
	{
		status = dispStr(floorDescriptionText[PIETRO][lang],y, reset);
		nameMode = FLOOR_NAME;
	}
	else if(code >= KOD_POZIOM_A && code <= KOD_POZIOM_J)
	{
		status = dispStr(floorDescriptionText[POZIOM][lang],y, reset);
		nameMode = FLOOR_NAME;
	}
	else if(code == KOD_PIETRO_0)
		{
			status = dispStr(floorDescriptionText[PARTER][lang],y, reset);
			nameMode = FLOOR_NAME;
		}

	return status;
}

void displayLargeFont(uint8_t code)
{
	nameMode = LARGE_FLOOR_NUMBER;

	const char litera[2] = { 'A' + (code - KOD_POZIOM_A), '\0' };

	if(code <= KOD_PIETRO_32)
		EVE_CMD_NUMBER(EVE_DISP_HEIGHT/2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code);

	if(code >= KOD_PIETRO_0B && code <= KOD_PIETRO_6B)
	{
		EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0B);
		EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
	}

	if(code >= KOD_PIETRO_0A && code <= KOD_PIETRO_9A)
	{
		EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0A);
		EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "A");
	}

	if(code == KOD_PIETRO_MIN_1_PRIM)
	{
		EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
		EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, 1);
		EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
	}

	if(code >= KOD_PIETRO_1_PRIM && code <= KOD_PIETRO_0_PRIM)
	{
		if(code == KOD_PIETRO_0_PRIM)
			EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, 0);
		else
			EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code - KOD_PIETRO_1_PRIM + 1);

		EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
	}

	if (code >= KOD_POZIOM_A && code <= KOD_POZIOM_J)
		EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, litera);

	if(code >= KOD_PIETRO_01 && code <= KOD_PIETRO_00)
	{
		EVE_CMD_NUMBER(-10, 16, LARGE_NUMBER, 0, 0);

		if(code == KOD_PIETRO_00)
			EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, 0);
		else
			EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_01 + 1);
	}

	if(code >= KOD_PIETRO_7B && code <= KOD_PIETRO_9B)
	{
		EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, (code - KOD_PIETRO_7B) + 7 );
		EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
	}

	if(code >= KOD_PIETRO_MINUS_1 && code <= KOD_PIETRO_MINUS_9)
	{
		EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
		EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, ((code - KOD_PIETRO_MINUS_1) + 1) );
	}

	if(code == KOD_JAZDA_ORIENTUJACA)
	{
		EVE_CMD_TEXT(2, 16, 2, 0, _QUE);
		EVE_CMD_TEXT(140, 16, 2, 0, _QUE);
	}
}

int showSpecialFloorName(uint8_t floorCode, int16_t y, int16_t yL1, int16_t yL2, uint8_t landscape, uint8_t flashing, uint8_t reset, uint8_t lang)
{
	uint8_t scroll = 0;

	if((floorCode >= KOD_SUTERENA_3 && floorCode <= KOD_PARKING) || floorCode == KOD_PIETRO_0)
		nameMode = SPECIAL_FLOOR;

	switch(floorCode)
	{
//		case KOD_PIETRO_0:				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, 2, EVE_OPT_CENTERX, floorSymbols[SYMBOL_PARTER][lang]);
//										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
//										scroll = dispStr(floorDescriptionText[PARTER][lang],y, reset);
//										break;

		case KOD_SUTERENA_3:			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 3);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[SUTERENA][lang],y, reset);
										break;

		case KOD_BALKON_3:				EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);  	EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[BALKON][lang],y, reset);
										break;

		case KOD_PARKING_3:				EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);  	EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);

										if(!flashing)
											EVE_CMD_BUTTON(100, 287, 75, 75, 31, 0, "P");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARKING][lang],y, reset);
										break;
		case KOD_PARKING_TECHNICZNY:	EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "T");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[0][lang],y, reset);
										break;

		case KOD_GARAZ_1:				EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);  	EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 1);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[GARAZ][lang],y, reset);
										break;

		case KOD_GARAZ_2:				EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);  	EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 2);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[GARAZ][lang],y, reset);
										break;

		case KOD_POZIOM_SPECJALNY:		//drawPlusSymbol(80,100,100,100,10);
										EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, _MINUS _MINUS );
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[1][lang],y, reset);
										break;

		case KOD_SZATNIA:				EVE_CMD_TEXT(-5, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SZATNIA][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[SZATNIA][lang],y, reset);
										break;

		case KOD_GARAZ:					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_GARAZ][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[GARAZ][lang],y, reset);
										break;

		case KOD_AMIFITEATR:			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, "A");  	EVE_CMD_TEXT(130, 16, 2, 0, "T");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[AMFITEATR][lang],y, reset);
										break;

		case KOD_BALKON_1:				EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[BALKON][lang],y, reset);
										break;

		case KOD_BALKON_2:				EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[BALKON][lang],y, reset);
										break;

		case KOD_BALKON:				EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_BALKON][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[BALKON][lang],y, reset);
										break;

		case KOD_PARKING_1:				EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);

										if(!flashing)
											EVE_CMD_BUTTON(100, 287, 75, 75, 31, 0, "P");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARKING][lang],y, reset);
										break;

		case KOD_PARKING_2:				EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);

										if(!flashing)
											EVE_CMD_BUTTON(100, 287, 75, 75, 31, 0, "P");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARKING][lang],y, reset);
										break;

		case KOD_PARTER_WYSOKI:			switch(lang)
										{
											case PL:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "W");
											break;

											case EN:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "U");
											break;

											case LANG_HIDDEN:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "U");
											break;
										}

										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[2][lang],y, reset);
										break;

		case KOD_DZWIG_NIECZYNNY:		if(landscape)
										{
											drawNoEntrySymbol(136,5,200);
											//if(lang != LANG_HIDDEN)
											{
												EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDA][lang]);
												EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NIECZYNNA][lang]);
											}

										}
										else
										{
											drawNoEntrySymbol(35,100,200);
											//if(lang != LANG_HIDDEN)
											{
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDA][lang]);
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NIECZYNNA][lang]);
											}

										}
										break;

		case KOD_JAZDA_ORIENTUJACA:		EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, _QUE);  	EVE_CMD_TEXT(140, 16, 2, 0, _QUE);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[4][lang],y, reset);
										break;

		case KOD_PARTER_NISKI:
										switch(lang)
										{
											case PL:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "N");
												break;

											case EN:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "L");
												break;

											case LANG_HIDDEN:
												EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "L");
												break;
										}

										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[5][lang],y, reset);
										break;
		case KOD_PRZECIAZENIE:
										#define OFFSET 180

										if(flashing % 2)
										{
											EVE_CMD_TEXT(-24, 16, BIG_ARTIFAKT_FONT, 0, "K");
											EVE_CMD_TEXT(116, 16, BIG_ARTIFAKT_FONT, 0, "G");
										}

										if(!landscape)
										{
											EVE_CMD_TEXT(20, 80 + OFFSET, 30, 0, "80%");
											EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 40 + OFFSET, 30, EVE_OPT_CENTERX, "100%");
											EVE_COLOR_RGB(255, 0, 0); // Czerwony kolor obwódki
											EVE_CMD_TEXT(180, 80 + OFFSET, 30, 0, "120%");
											EVE_CMD_GAUGE(EVE_DISP_HEIGHT/2, 160 + OFFSET, 100, EVE_OPT_NOBACK, 1, 4, 96, 128);
										}
										else
										{
											if(flashing % 2)
												drawSimpleArrowUp(10, 380, 50, 120, 40);
										}

										EVE_COLOR_RGB(255, 255, 255);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[6][lang],y, reset);
										break;

		case KOD_JAZDA_SPECJALNA:		//if(flashing % 2) //(flashing % 2)
										//{
											EVE_CMD_TEXT(-24, 16, BIG_ARTIFAKT_FONT, 0, "E");
											EVE_CMD_TEXT(110, 16, BIG_ARTIFAKT_FONT, 0, "X");
										//}
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorSpecDescriptions[7][lang],y, reset);
										break;

		case KOD_HOL:					EVE_CMD_TEXT(-10, 16, BIG_ARTIFAKT_FONT, 0, "H");  	EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "L");
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[HOL][lang],y, reset);
										break;

		case KOD_SUTERENENA:			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_SUTERENA][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[SUTERENA][lang],y, reset);
										break;

		case KOD_SUTERENA_1:			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[SUTERENA][lang],y, reset);
										break;

		case KOD_SUTERENA_2:			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[SUTERENA][lang],y, reset);
										break;

		case KOD_ANTRESOLA:				EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[ANTRESOLA][lang],y, reset);
										break;

		case KOD_ANTRESOLA_1:
										switch(lang)
										{
											case PL:
												EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
												EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 1);
												break;
											case EN:
												if(landscape)
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
													EVE_CMD_NUMBER(180, 16, LARGE_NUMBER, 0, 1);
												}
												else
												{

													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);

													if(!flashing)
														EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 1);
												}
												break;

											case LANG_HIDDEN:
												if(landscape)
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
													EVE_CMD_NUMBER(180, 16, LARGE_NUMBER, 0, 1);
												}
												else
												{

													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);

													if(!flashing)
														EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 1);
																								}
												break;

										}

										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[ANTRESOLA][lang],y, reset);
										break;

		case KOD_ANTRESOLA_2:
										switch(lang)
										{
											case PL:
												EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
												EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 2);
												break;
											case EN:
												if(landscape)
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
													EVE_CMD_NUMBER(175, 16, LARGE_NUMBER, 0, 2);
												}
												else
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);

													if(!flashing)
														EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 2);
												}
												break;
											case LANG_HIDDEN:
												if(landscape)
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
													EVE_CMD_NUMBER(175, 16, LARGE_NUMBER, 0, 2);
												}
												else
												{
													EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);

													if(!flashing)
														EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 2);
												}
												break;
										}
										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[ANTRESOLA][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[ANTRESOLA][lang],y, reset);
										break;

		case KOD_RECEPCJA:				EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "R");
										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[RECEPCJA][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[RECEPCJA][lang],y, reset);
										break;

		case KOD_PARTER:				EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_PARTEROW][lang]);
										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARTER][lang],y, reset);
										break;

		case KOD_PARTER_1:				EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARTER][lang],y, reset);
										break;

		case KOD_PARTER_2:				EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);  	EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
										EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARTER][lang],y, reset);
										break;

		case KOD_PARKING:				EVE_CMD_TEXT(-24, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);  	EVE_CMD_TEXT(110, 16, 2, 0, "A");

										if(!flashing)
											EVE_CMD_BUTTON(100, 287, 75, 75, 31, 0, "P");
										//EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARKING][lang]);
										//if(lang != LANG_HIDDEN)
											scroll = dispStr(floorDescriptionText[PARKING][lang],y, reset);
										break;
	}

	return scroll;
}

int showExtraFunctionCode(uint8_t Code, uint8_t landscape, int16_t y, int16_t yL1, int16_t yL2, uint8_t reset, uint8_t lang)
{
	uint8_t status = 0;

	EVE_CLEAR(255,255,255);

	switch(Code)
	{
		case KOD_JAZDA_KONTROLNA:
			if(landscape)
			{
				drawStairsSymbol(240, 60, 200, 100, 4);
				drawArrowSymbol(30, 50, 150, 100);
				//if(lang != LANG_HIDDEN)
				{
					EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NAPRAWA][lang]);
					EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDY][lang]);
				}
			}
			else
			{
				drawStairsSymbol(40, 220, 200, 100, 4);
				drawArrowSymbol(50, 60, 150, 100);
				//if(lang != LANG_HIDDEN)
				{
					EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NAPRAWA][lang]);
					EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDY][lang]);
				}
			}
			break;

		case KOD_ZJAZD_AWARYJNY:
				if(landscape) drawBatterySymbol(200, 40, 90,150,75);
				else drawBatterySymbol(70,100, 130,200,75);
				//if(lang != LANG_HIDDEN)
					status = dispStr(floorSpecDescriptions[9][lang],y, reset);
			break;

		case KOD_REMONT_WINDY:
				drawStairsSymbol(40, 60, 200, 100, 4);
				if(landscape) DrawTriangleWithExclamation(3600, 1, 40);
				else DrawTriangleWithExclamation(260, 160 * 16, 40);
				//if(lang != LANG_HIDDEN)
					status = dispStr(floorSpecDescriptions[10][lang],y, reset);
			break;

		case KOD_JAZDA_POZAROWA:			/*45000*/
				if(landscape)DrawFlameSymbol(450*6, EVE_DISP_HEIGHT/2, 20);
				else DrawFlameSymbol(350, 34*16, 30);
				//if(lang != LANG_HIDDEN)
					status = dispStr(floorSpecDescriptions[11][lang],y, reset);
			break;
	}

	return status;
}

int showErrorName(uint8_t lastCode, uint8_t landscape, int16_t y, int16_t yL1, uint8_t reset, uint8_t lang)
{
	uint8_t status = 0;
	nameMode = ERROR_NAME;

	if(lastCode <= 49)
	{
		int16_t x,y1,xL1, xL2;

		x = EVE_DISP_HEIGHT/2;
		xL1 = EVE_DISP_HEIGHT/2;
		y1 = 16;
		yL1 = 220;

		if(landscape)
		{
			x = 90;
			xL1 = 310;
			yL1 = 16;
		}

		EVE_CLEAR(255,255,255);
		EVE_COLOR_RGB(255, 0, 0);
		EVE_CMD_TEXT(x, y1, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "E");
		EVE_COLOR_RGB(255, 255, 255);

		if(lastCode < 10)
		{
			if(landscape)
			{
				xL1 = 150;
				xL2 = 320;
			}
			else
			{
				xL1 = -10;
				xL2 = 130;
			}

			EVE_CMD_NUMBER(xL1, yL1, LARGE_NUMBER, 0, 0);
			EVE_CMD_NUMBER(xL2, yL1, LARGE_NUMBER, 0, lastCode);
		}
		else
		{
			EVE_CMD_NUMBER(xL1, yL1, LARGE_NUMBER, EVE_OPT_CENTERX, lastCode);
		}

				//scroll = scrollText(errorDescriptionText[lastCode][lang], 0, y, 480, 3, 500, reset);
		//if (lang != LANG_HIDDEN)
			status =  dispStr(errorDescriptionText[lastCode][lang],y, reset);
	}

	return status;
}

int outOforder(uint8_t lastCode, int16_t yL1, int16_t yL2, uint8_t landscape, uint8_t lang)
{
	EVE_CLEAR(255,255,255);

	switch(lastCode)
	{
		case KOD_DZWIG_NIECZYNNY:
			if(landscape)
				{
					drawNoEntrySymbol(136,5,200);
					//if(lang != LANG_HIDDEN)
					{
						EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDA][lang]);
						EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NIECZYNNA][lang]);
					}
				}
				else
				{
					drawNoEntrySymbol(35,100,200);
					//if(lang != LANG_HIDDEN)
					{
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL1, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[WINDA][lang]);
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[NIECZYNNA][lang]);
					}
				}
				break;

		case KOD_BRAK_LIFTCAN:
				if(landscape)
				{
					drawNoEntrySymbol(136,5,200);
					//if(lang != LANG_HIDDEN)
						EVE_CMD_TEXT(EVE_DISP_WIDTH/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[LIFTCAN_NIE_WYKRYTY][lang]);

				}
				else
				{
					drawNoEntrySymbol(35,100,200);
					//if(lang != LANG_HIDDEN)
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, yL2, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[LIFTCAN_NIE_WYKRYTY][lang]);
				}
				break;
	}

	return 0;
}

void decodeCodeFloor(uint8_t* canFloorCode, uint8_t* flashing, uint8_t *errorDetect, uint8_t lang, uint8_t lastCode)
{
	#define SECOND 1
	int16_t y, yL1, yL2 = 0;
	uint8_t landscape = 0;
	uint8_t floorCode = *canFloorCode;
	uint8_t flash = *flashing;
	uint8_t errorDet = *errorDetect;
	uint8_t resetScrollFloorNames = false;
	uint8_t resetScrollSpecialFloor = false;
	uint8_t resetScroolErrorNames = false;
	uint8_t resetScroolExtraNames = false;

	EVE_CMD_ROMFONT(BUILT_FONT, 31);
	EVE_CMD_SETFONT2(BIG_ARTIFAKT_FONT, RAM_G_LARGE_FONT_OFFSET,65);
	EVE_CMD_SETFONT2(SMALL_PL_FONT, RAM_G_PL_FONT_OFFSET, 65);
	EVE_CMD_SETFONT2(LARGE_NUMBER, RAM_G_LARGE_NUMBER_OFFSET, 48);

	if(nodeParameters[ROTATE].wartosc == 0 || nodeParameters[ROTATE].wartosc == 1)
	{
		y = 230;
		landscape = 1;
		yL1 = 200;
		yL2 = 230;
	}
	else
	{
		y = 430;
		landscape = 0;
		yL1 = 390;
		yL2 = 430;
	}

	if(errorDet == ERROR_DETECT)
	{
		scrollFloorNames = scrollSpecialFloor = false;

		if(!scroolErrorNames)
			resetScroolErrorNames = true;

		if(global.CanByteSeg3 == KOD_JAZDA_ORIENTUJACA)
			floorCode = KOD_JAZDA_ORIENTUJACA;

		/*if(!scroolErrorNames && ownSec) displayLargeFont(floorCode);
		else scroolErrorNames = showErrorName(lastCode, landscape, y, yL1, resetScroolErrorNames, lang), ownSec = SECOND;*/
		/*if(!scroolErrorNames && ownSec && lang != LANG_HIDDEN) { //na razie podcas lang hidden nie wyswietla pietra 23.07.2025
			displayLargeFont(floorCode);
		}
		else{
			scroolErrorNames = showErrorName(lastCode, landscape, y, yL1, resetScroolErrorNames, lang);
			ownSec = SECOND;
		}*/

		if(!scroolErrorNames && ownSec) {
					//displayLargeFont(floorCode);
					displayInError(floorCode, y, landscape, lang);
					langtime = 3;
				}
				else {
					if(lang == LANG_HIDDEN){
						scroolErrorNames = showErrorName(lastCode, landscape, y, yL1, resetScroolErrorNames, lang);
						if(!langtime)
							ownSec = SECOND;
					}
					else {
					scroolErrorNames = showErrorName(lastCode, landscape, y, yL1, resetScroolErrorNames, lang);
					ownSec = SECOND;
					}
				}
		if(global.CanByteSeg1 == KOD_JAZDA_SPECJALNA)
		{
						if(landscape){
							EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, "E");
							EVE_CMD_TEXT(320, 16, BIG_ARTIFAKT_FONT, 0, "X");
						}
						else
						{
							EVE_CMD_TEXT(-24, 220, BIG_ARTIFAKT_FONT, 0, "E");
							EVE_CMD_TEXT(110, 220, BIG_ARTIFAKT_FONT, 0, "X");
						}

						scroolErrorNames = displayInExpressMode(floorCode, y, landscape, resetScroolErrorNames, lang);
						//displayLargeFont(floorCode);
						//showFloorOrLevel(floorCode, y, resetScrollFloorNames, lang);
						//showSpecialFloorName(floorCode, y, yL1, yL2, landscape, flash, resetScrollSpecialFloor, lang); //powoduje wyswietlanie napisu na dole
		}


	}

	else if (lastCode == KOD_DZWIG_NIECZYNNY || lastCode == KOD_BRAK_LIFTCAN)
	{
		outOforder(lastCode, yL1, yL2, landscape, lang);
	}
	else
	{
		if(!scrollFloorNames) resetScrollFloorNames = true;
		if(!scrollSpecialFloor) resetScrollSpecialFloor = true;

		/*floorCode*/
		displayLargeFont(lastCode);
		//if(lang != LANG_HIDDEN)
			scrollFloorNames = showFloorOrLevel(lastCode, y, resetScrollFloorNames, lang);
		scrollSpecialFloor = showSpecialFloorName(lastCode, y, yL1, yL2, landscape, flash, resetScrollSpecialFloor, lang);

	}

	if(lastCode >= KOD_JAZDA_KONTROLNA && (lastCode != KOD_BRAK_LIFTCAN && lastCode != KOD_DZWIG_NIECZYNNY))
	{
		scrollFloorNames = scrollSpecialFloor = false;

		if(!scroolExtraNames) resetScroolExtraNames = true;

		if(!scroolExtraNames && sec) displayLargeFont(floorCode);
		else scroolExtraNames = showExtraFunctionCode(lastCode, landscape, y, yL1, yL2, resetScroolExtraNames, lang);
	}
}

void drawBatterySymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t chargeLevel)
{
    uint16_t terminalWidth = width / 4;
    uint16_t terminalHeight = height / 8;
    uint16_t chargeHeight = (height - 2) * chargeLevel / 100; // Oblicz wysokość naładowania
    uint16_t borderThickness = 5; // Grubość obwódki

    EVE_LINE_WIDTH(10 * 16); // Ustawienie szerokości drzwi

    // Rysowanie obwódki baterii
    EVE_COLOR_RGB(255, 0, 0); // Czerwony kolor obwódki
    EVE_BEGIN(9);
    EVE_VERTEX2F((x - borderThickness) * 16, (y - borderThickness) * 16); // Lewy górny róg obwódki
    EVE_VERTEX2F((x + width + borderThickness) * 16, (y + height + borderThickness) * 16); // Prawy dolny róg obwódki
    EVE_END();

    // Rysowanie obwódki terminala
    EVE_COLOR_RGB(255, 0, 0); // Czerwony kolor obwódki
    EVE_BEGIN(9);
    EVE_VERTEX2F(
        ((x + width / 2 - terminalWidth / 2) - borderThickness) * 16,
        (y - terminalHeight - borderThickness) * 16); // Lewy górny róg obwódki terminala
    EVE_VERTEX2F(
        ((x + width / 2 + terminalWidth / 2) + borderThickness) * 16,
        (y + borderThickness) * 16); // Prawy dolny róg obwódki terminala
    EVE_END();

    // Rysowanie konturu baterii
    EVE_COLOR_RGB(255, 255, 255); // Biały kolor konturu
    EVE_BEGIN(9);
    EVE_VERTEX2F(x * 16, y * 16); // Lewy górny róg baterii
    EVE_VERTEX2F((x + width) * 16, (y + height) * 16); // Prawy dolny róg baterii

    // Rysowanie terminala
    EVE_VERTEX2F((x + width / 2 - terminalWidth / 2) * 16, (y - terminalHeight) * 16);
    EVE_VERTEX2F((x + width / 2 + terminalWidth / 2) * 16, y * 16); // Terminal
    EVE_END();

    // Wypełnienie poziomu naładowania w pionie
    EVE_COLOR_RGB(0, 255, 0); // Zielony kolor
    EVE_BEGIN(9);
    EVE_VERTEX2F((x + 1) * 16, ((y + height - 1) - chargeHeight) * 16); // Dolna granica naładowania
    EVE_VERTEX2F((x + width - 1) * 16, (y + height - 1) * 16); // Górna granica
    EVE_END();

    // Powrót koloru na biały
    EVE_COLOR_RGB(255, 255, 255);
}

void DrawFlameSymbol(int x, int y, int size)
{
	EVE_COLOR_RGB(255, 0, 0);

	// Główne punkty płomienia
	EVE_BEGIN(2);
	EVE_POINT_SIZE(40 * size); // Ustawienie rozmiaru punktu
	EVE_VERTEX2F(x + 60 * size, y + 60 * size);
	EVE_POINT_SIZE(30 * size);
	EVE_VERTEX2F(x + 55 * size, y + 40 * size);
	EVE_POINT_SIZE(20 * size);
	EVE_VERTEX2F(x + 50 * size, y + 20 * size);
	EVE_POINT_SIZE(10 * size);
	EVE_VERTEX2F(x + 45 * size, y + 10 * size);
	EVE_END();

	// Mniejsze płomienie
	EVE_COLOR_RGB(255, 102, 0);
	EVE_BEGIN(2);
	EVE_POINT_SIZE(30 * size);
	EVE_VERTEX2F(x + 50 * size, y + 70 * size);
	EVE_POINT_SIZE(20 * size);
	EVE_VERTEX2F(x + 45 * size, y + 50 * size);
	EVE_POINT_SIZE(10 * size);
	EVE_VERTEX2F(x + 40 * size, y + 30 * size);
	EVE_END();

	EVE_COLOR_RGB(255, 204, 0);
	EVE_BEGIN(2);
	EVE_POINT_SIZE(20 * size);
	EVE_VERTEX2F(x + 55 * size, y + 80 * size);
	EVE_POINT_SIZE(10 * size);
	EVE_VERTEX2F(x + 50 * size, y + 60 * size);
	EVE_POINT_SIZE(5 * size);
	EVE_VERTEX2F(x + 45 * size, y + 40 * size);
	EVE_END();

	// Rysowanie symbolu małego domku, obniżonego o 50 jednostek
	int houseOffset = 400; // Offset, aby dach był poniżej płomienia
	EVE_COLOR_RGB(255, 255, 255);
	EVE_LINE_WIDTH(8 * 16); // Ustawienie szerokości linii
	EVE_BEGIN(3);
	// Podstawa domku
	EVE_VERTEX2F(x + 30 * size, y + houseOffset + 140 * size);
	EVE_VERTEX2F(x + 90 * size, y + houseOffset + 140 * size);
	// Ściany domku
	EVE_VERTEX2F(x + 30 * size, y + houseOffset + 140 * size);
	EVE_VERTEX2F(x + 30 * size, y + houseOffset + 110 * size);
	EVE_VERTEX2F(x + 90 * size, y + houseOffset + 140 * size);
	EVE_VERTEX2F(x + 90 * size, y + houseOffset + 110 * size);
	// Dach domku
	EVE_VERTEX2F(x + 30 * size, y + houseOffset + 110 * size);
	EVE_VERTEX2F(x + 60 * size, y + houseOffset + 80 * size);
	EVE_VERTEX2F(x + 60 * size, y + houseOffset + 80 * size);
	EVE_VERTEX2F(x + 90 * size, y + houseOffset + 110 * size);
	EVE_END();

	// Rysowanie drzwi jako linia
	EVE_LINE_WIDTH(10 * 16); // Ustawienie szerokości drzwi
	EVE_BEGIN(3);
	EVE_VERTEX2F(x + 60 * size, y + houseOffset + 135 * size); // Skrócenie drzwi o 5 jednostek od dołu
	EVE_VERTEX2F(x + 60 * size, y + houseOffset + 120 * size);
	EVE_END();
}

void drawStairsSymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t stepCount)
{
    uint16_t stepWidth = width / stepCount;    // Szerokość pojedynczego stopnia
    uint16_t stepHeight = height / stepCount; // Wysokość pojedynczego stopnia

    EVE_COLOR_RGB(255, 255, 255); // Biały kolor schodów
    EVE_LINE_WIDTH(10 * 16); // Ustawienie szerokości drzwi

    for (uint16_t i = 0; i < stepCount; i++)
    {
        // Rysowanie każdego stopnia
        uint16_t stepX = x + i * stepWidth;
        uint16_t stepY = y + (stepCount - i - 1) * stepHeight;

        EVE_BEGIN(9); // Rysowanie prostokąta dla stopnia
        EVE_VERTEX2F(stepX * 16, stepY * 16);                             // Lewy górny róg
        EVE_VERTEX2F((stepX + stepWidth) * 16, (stepY + stepHeight) * 16); // Prawy dolny róg
        EVE_END();
    }

    // Powrót koloru do domyślnego
    EVE_COLOR_RGB(255, 255, 255);
}


void drawNoEntrySymbol(uint16_t x, uint16_t y, uint16_t diameter)
{
    uint16_t radius = diameter / 2;           // Promień koła
    uint16_t borderThickness = diameter / 10; // Grubość obramowania
    uint16_t stripeHeight = diameter / 10;     // Wysokość białego paska

    EVE_COLOR_RGB(255, 0, 0); // Czerwony kolor
    EVE_BEGIN(2);
    EVE_POINT_SIZE(radius * 16); // Rozmiar koła
    EVE_VERTEX2F((x + radius) * 16, (y + radius) * 16); // Środek koła
    EVE_END();

    EVE_COLOR_RGB(255, 255, 255); // Biały kolor
    EVE_BEGIN(3); // Tryb linii
    EVE_LINE_WIDTH(stripeHeight * 16); // Grubość poziomego paska
    EVE_VERTEX2F((x + borderThickness + 10) * 16, (y + radius) * 16); // Lewy punkt paska
    EVE_VERTEX2F((x + diameter - borderThickness - 10) * 16, (y + radius) * 16); // Prawy punkt paska

    EVE_END();

    EVE_COLOR_RGB(255, 255, 255);
}

void DrawTriangleWithExclamation(int x, int y, int size)
{
    EVE_COLOR_RGB(255, 255, 0); // Żółty trójkąt

    // Rysowanie trójkąta
    EVE_LINE_WIDTH(8 * 16); // Ustawienie szerokości linii
    EVE_BEGIN(3);
    EVE_VERTEX2F(x + 50 * size, y + 20 * size);
    EVE_VERTEX2F(x + 80 * size, y + 80 * size);
    EVE_VERTEX2F(x + 80 * size, y + 80 * size);
    EVE_VERTEX2F(x + 20 * size, y + 80 * size);
    EVE_VERTEX2F(x + 20 * size, y + 80 * size);
    EVE_VERTEX2F(x + 50 * size, y + 20 * size);
    EVE_END();

    EVE_COLOR_RGB(255, 0, 0); // Czerwony wykrzyknik

    // Rysowanie wykrzyknika
    EVE_LINE_WIDTH(4 * 16); // Ustawienie szerokości linii
    EVE_BEGIN(3);
    EVE_VERTEX2F(x + 50 * size, y + 35 * size);
    EVE_VERTEX2F(x + 50 * size, y + 60 * size);
    EVE_END();

    EVE_BEGIN(2);
    EVE_POINT_SIZE(5 * size); // Ustawienie rozmiaru punktu
    EVE_VERTEX2F(x + 50 * size, y + 70 * size); // Kropka wykrzyknika
    EVE_END();
    EVE_COLOR_RGB(255, 255, 255); // Żółty trójkąt
}

int scrollText (const char *text, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t speed, uint16_t gap, uint8_t reset)
{
	 static int16_t positionX = 0; // Aktualna pozycja tekstu
	 uint16_t textWidth = 272;
	 int8_t scrollStatus = 1; // 1 - przewijanie aktywne, 0 - zakończone

	 if (reset)
	 {
		 positionX = endX; // Resetuj pozycję na początek przewijania
	 }

	 if (positionX + textWidth + gap < startX)
	 {
	    //positionX = endX; // Resetuj na prawą krawędź ekranu
	    scrollStatus = 0; // Przewijanie zakończone
	    return scrollStatus;
	 }

	 EVE_CMD_TEXT(positionX, startY, SMALL_PL_FONT, 0, text);
	 positionX -= speed; // Przesuń tekst w lewo

	 return scrollStatus; // Zwróć status przewijania
}

void drawArrowSymbol(uint16_t x, uint16_t y, uint16_t length, uint16_t height)
{
    EVE_BEGIN(3); // Rozpocznij rysowanie linii

    EVE_COLOR_RGB(255, 0, 0);
    EVE_LINE_WIDTH(200);

    // Linia pozioma strzałki
    EVE_VERTEX2F(x * 16, (y + height / 2) * 16);
    EVE_VERTEX2F((x + length) * 16, (y + height / 2) * 16);

    // Lewa część grotu strzałki
    EVE_VERTEX2F((x + length) * 16, (y + height / 2) * 16);
    EVE_VERTEX2F((x + length - height / 2) * 16, (y + height) * 16);

    // Prawa część grotu strzałki
    EVE_VERTEX2F((x + length) * 16, (y + height / 2) * 16);
    EVE_VERTEX2F((x + length - height / 2) * 16, y * 16);
    EVE_COLOR_RGB(255, 255, 255);

    EVE_END(); // Zakończ rysowanie
}

int dispStr(const char* str, int16_t y, uint8_t reset)
{
	uint8_t scrollStat = 0;

	if(str != NULL)
	{
		if(strlen(str) > MAX_STR_LEN_HORI)
			scrollStat = scrollText(str, 0, y, 480, 3, 500, reset);
		else
			EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, str);
	}

	return scrollStat;
}

void drawSimpleArrowUp(int line_width, int x, int y, int arrow_height, int arrow_width)
{
    // Ustaw kolor linii z uwzględnieniem przezroczystości (alpha)
    EVE_COLOR_RGB(255, 255, 255);
    // Ustaw szerokość linii
    EVE_LINE_WIDTH(line_width * 16);  // Szerokość linii ustawiona w jednostkach fixpoint
    // Rozpocznij rysowanie linii
    EVE_BEGIN(4);
    // Rysuj pionową linię strzałki w górę
    EVE_VERTEX2F(x * 16, (y + arrow_height) * 16);    // Punkt początkowy (na dole)
    EVE_VERTEX2F(x * 16, y * 16);                    // Punkt końcowy (na górze)
    // Rysuj lewą skośną linię grotu strzałki
    EVE_VERTEX2F(x * 16, y * 16);                                  // Punkt początkowy na końcu pionowej linii (góra)
    EVE_VERTEX2F((x - arrow_width) * 16, (y + (arrow_height / 3)) * 16);  // Punkt końcowy lewej linii (w dół)
    // Rysuj prawą skośną linię grotu strzałki
    EVE_VERTEX2F(x * 16, y * 16);                                  // Punkt początkowy na końcu pionowej linii (góra)
    EVE_VERTEX2F((x + arrow_width) * 16, (y + (arrow_height / 3)) * 16);  // Punkt końcowy prawej linii (w dół)
    // Zakończ rysowanie linii
    EVE_END();
}

int displayInExpressMode(uint8_t code, int16_t y, uint8_t landscape, uint8_t reset, uint8_t lang)
{
	uint8_t scroll = 0;
	const char litera[2] = { 'A' + (code - KOD_POZIOM_A), '\0' };

		if(code <= KOD_PIETRO_32)
			EVE_CMD_NUMBER(EVE_DISP_HEIGHT/2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code);

		if(code >= KOD_PIETRO_0B && code <= KOD_PIETRO_6B)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0B);
			EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
		}

		if(code >= KOD_PIETRO_0A && code <= KOD_PIETRO_9A)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0A);
			EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "A");
		}

		if(code == KOD_PIETRO_MIN_1_PRIM)
		{
			EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
			EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, 1);
			EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
		}

		if(code >= KOD_PIETRO_1_PRIM && code <= KOD_PIETRO_0_PRIM)
		{
			if(code == KOD_PIETRO_0_PRIM)
				EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, 0);
			else
				EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code - KOD_PIETRO_1_PRIM + 1);

			EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
		}

		if (code >= KOD_POZIOM_A && code <= KOD_POZIOM_J)
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, litera);

		if(code >= KOD_PIETRO_01 && code <= KOD_PIETRO_00)
		{
			EVE_CMD_NUMBER(-10, 16, LARGE_NUMBER, 0, 0);

			if(code == KOD_PIETRO_00)
				EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, 0);
			else
				EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_01 + 1);
		}

		if(code >= KOD_PIETRO_7B && code <= KOD_PIETRO_9B)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, (code - KOD_PIETRO_7B) + 7 );
			EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
		}

		if (code == KOD_SUTERENA_3) {
			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_BALKON_3) {
			EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_PARKING_3) {
			EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_PARKING_TECHNICZNY) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "T");
		}
		if (code == KOD_GARAZ_1) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);
			EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_GARAZ_2) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);
			EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_POZIOM_SPECJALNY) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, _MINUS _MINUS);
		}
		if (code == KOD_SZATNIA) {
			EVE_CMD_TEXT(-5, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SZATNIA][lang]);
		}
		if (code == KOD_GARAZ) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_GARAZ][lang]);
		}
		if (code == KOD_AMIFITEATR) {
			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, "A");
			EVE_CMD_TEXT(130, 16, 2, 0, "T");
		}
		if (code == KOD_BALKON_1) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_BALKON_2) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_BALKON) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_BALKON][lang]);
		}
		if (code == KOD_PARKING_1) {
			EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_PARKING_2) {
			EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_PARTER_WYSOKI) {
			if (lang == PL) {
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "W");
			}
			else if (lang == EN || lang == LANG_HIDDEN) {
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "U");
			}
		}

		if(code >= KOD_PIETRO_MINUS_1 && code <= KOD_PIETRO_MINUS_9)
		{
			EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
			EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, ((code - KOD_PIETRO_MINUS_1) + 1) );
		}

		if (code == KOD_PARTER_NISKI) {
					if (lang == PL) {
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "N");
					}
					else if (lang == EN || lang == LANG_HIDDEN) {
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "L");
					}
				}
		if (code == KOD_HOL) {
					EVE_CMD_TEXT(-10, 16, BIG_ARTIFAKT_FONT, 0, "H");
					EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "L");
				}
		if (code == KOD_SUTERENENA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_SUTERENA][lang]);
				}
		if (code == KOD_SUTERENA_1) {
					EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
				}
		if (code == KOD_SUTERENA_2) {
					EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
				}
		if (code == KOD_ANTRESOLA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
				}
		if (code == KOD_ANTRESOLA_1) {
					if (lang == PL) {
						EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
						EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 1);
					}
					if (lang == EN || lang == LANG_HIDDEN) {
						if (landscape) {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(180, 16, LARGE_NUMBER, 0, 1);
						} else {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 1);
						}
					}
				}
		if (code == KOD_ANTRESOLA_2) {
					if (lang == PL) {
						EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
						EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 2);
					}
					else if (lang == EN || lang == LANG_HIDDEN) {
						if (landscape) {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(175, 16, LARGE_NUMBER, 0, 2);
						} else {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 2);
						}
					}
				}
		if (code == KOD_RECEPCJA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "R");
				}
		if (code == KOD_PARTER) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_PARTEROW][lang]);
				}
		if (code == KOD_PARTER_1) {
					EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
				}
		if (code == KOD_PARTER_2) {
					EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
					EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
				}
		if (code == KOD_PARKING) {
					EVE_CMD_TEXT(-24, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
					EVE_CMD_TEXT(110, 16, 2, 0, "A");
				}
		scroll=dispStr(floorSpecDescriptions[7][lang],y, reset);

		return scroll;
}

void displayInError(uint8_t code,int16_t y, uint8_t landscape, uint8_t lang)
{
	const char litera[2] = { 'A' + (code - KOD_POZIOM_A), '\0' };

		if(code <= KOD_PIETRO_32)
			EVE_CMD_NUMBER(EVE_DISP_HEIGHT/2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code);

		if(code >= KOD_PIETRO_0B && code <= KOD_PIETRO_6B)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0B);
			EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
		}

		if(code >= KOD_PIETRO_0A && code <= KOD_PIETRO_9A)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_0A);
			EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "A");
		}

		if(code == KOD_PIETRO_MIN_1_PRIM)
		{
			EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
			EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, 1);
			EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
		}

		if(code >= KOD_PIETRO_1_PRIM && code <= KOD_PIETRO_0_PRIM)
		{
			if(code == KOD_PIETRO_0_PRIM)
				EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, 0);
			else
				EVE_CMD_NUMBER(EVE_DISP_HEIGHT / 2, 16, LARGE_NUMBER, EVE_OPT_CENTERX, code - KOD_PIETRO_1_PRIM + 1);

			EVE_CMD_TEXT(200, 16, BIG_ARTIFAKT_FONT, 0, _PRIM);
		}

		if (code >= KOD_POZIOM_A && code <= KOD_POZIOM_J)
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, litera);

		if(code >= KOD_PIETRO_01 && code <= KOD_PIETRO_00)
		{
			EVE_CMD_NUMBER(-10, 16, LARGE_NUMBER, 0, 0);

			if(code == KOD_PIETRO_00)
				EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, 0);
			else
				EVE_CMD_NUMBER(124, 16, LARGE_NUMBER, 0, code - KOD_PIETRO_01 + 1);
		}

		if(code >= KOD_PIETRO_7B && code <= KOD_PIETRO_9B)
		{
			EVE_CMD_NUMBER(-5, 16, LARGE_NUMBER, 0, (code - KOD_PIETRO_7B) + 7 );
			EVE_CMD_TEXT(120, 16, BIG_ARTIFAKT_FONT, 0, "B");
		}

		if (code == KOD_SUTERENA_3) {
			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_BALKON_3) {
			EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_PARKING_3) {
			EVE_CMD_TEXT(-20, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(130, 16, LARGE_NUMBER, 0, 3);
		}
		if (code == KOD_PARKING_TECHNICZNY) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "T");
		}
		if (code == KOD_GARAZ_1) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);
			EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_GARAZ_2) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_GARAZ][lang]);
			EVE_CMD_NUMBER(145, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_POZIOM_SPECJALNY) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, _MINUS _MINUS);
		}
		if (code == KOD_SZATNIA) {
			EVE_CMD_TEXT(-5, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SZATNIA][lang]);
		}
		if (code == KOD_GARAZ) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_GARAZ][lang]);
		}
		if (code == KOD_AMIFITEATR) {
			EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, "A");
			EVE_CMD_TEXT(130, 16, 2, 0, "T");
		}
		if (code == KOD_BALKON_1) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_BALKON_2) {
			EVE_CMD_TEXT(-8, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_BALKON][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_BALKON) {
			EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_BALKON][lang]);
		}
		if (code == KOD_PARKING_1) {
			EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
		}
		if (code == KOD_PARKING_2) {
			EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
			EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
		}
		if (code == KOD_PARTER_WYSOKI) {
			if (lang == PL) {
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "W");
			}
			else if (lang == EN || lang == LANG_HIDDEN) {
				EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "U");
			}
		}

		if(code >= KOD_PIETRO_MINUS_1 && code <= KOD_PIETRO_MINUS_9)
		{
			EVE_CMD_TEXT(0, 16, BIG_ARTIFAKT_FONT, 0, _MINUS);
			EVE_CMD_NUMBER(100, 16, LARGE_NUMBER, 0, ((code - KOD_PIETRO_MINUS_1) + 1) );
		}

		if (code == KOD_PARTER_NISKI) {
					if (lang == PL) {
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "P");
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 158, BUILT_FONT, 0, "N");
					}
					else if (lang == EN || lang == LANG_HIDDEN) {
						EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "L");
					}
				}
		if (code == KOD_HOL) {
					EVE_CMD_TEXT(-10, 16, BIG_ARTIFAKT_FONT, 0, "H");
					EVE_CMD_TEXT(130, 16, BIG_ARTIFAKT_FONT, 0, "L");
				}
		if (code == KOD_SUTERENENA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_SUTERENA][lang]);
				}
		if (code == KOD_SUTERENA_1) {
					EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
				}
		if (code == KOD_SUTERENA_2) {
					EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_SUTERENA][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
				}
		if (code == KOD_ANTRESOLA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
				}
		if (code == KOD_ANTRESOLA_1) {
					if (lang == PL) {
						EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
						EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 1);
					}
					if (lang == EN || lang == LANG_HIDDEN) {
						if (landscape) {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(180, 16, LARGE_NUMBER, 0, 1);
						} else {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 1);
						}
					}
				}
		if (code == KOD_ANTRESOLA_2) {
					if (lang == PL) {
						EVE_CMD_TEXT(2, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_ANTRESOLA][lang]);
						EVE_CMD_NUMBER(150, 16, LARGE_NUMBER, 0, 2);
					}
					else if (lang == EN || lang == LANG_HIDDEN) {
						if (landscape) {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 3, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(175, 16, LARGE_NUMBER, 0, 2);
						} else {
							EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_ANTRESOLA][lang]);
							EVE_CMD_NUMBER(75, 220, LARGE_NUMBER, 0, 2);
						}
					}
				}
		if (code == KOD_RECEPCJA) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, "R");
				}
		if (code == KOD_PARTER) {
					EVE_CMD_TEXT(EVE_DISP_HEIGHT / 2, 16, BIG_ARTIFAKT_FONT, EVE_OPT_CENTERX, floorSymbols[SYMBOL_PARTEROW][lang]);
				}
		if (code == KOD_PARTER_1) {
					EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 1);
				}
		if (code == KOD_PARTER_2) {
					EVE_CMD_TEXT(-4, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARTEROW][lang]);
					EVE_CMD_NUMBER(140, 16, LARGE_NUMBER, 0, 2);
					EVE_CMD_TEXT(EVE_DISP_HEIGHT/2, y, SMALL_PL_FONT, EVE_OPT_CENTERX, floorDescriptionText[PARTER][lang]);
				}
		if (code == KOD_PARKING) {
					EVE_CMD_TEXT(-24, 16, BIG_ARTIFAKT_FONT, 0, floorSymbols[SYMBOL_PARKING][lang]);
					EVE_CMD_TEXT(110, 16, 2, 0, "A");
				}

}

//void __attribute__((optimize("-O1"))) drawPlusSymbol(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t borderThickness)
//{
//    uint16_t innerWidth = width - 2 * borderThickness; // Szerokość wewnętrznego plusa
//    uint16_t innerHeight = height - 2 * borderThickness; // Wysokość wewnętrznego plusa
//
//    // Rysowanie białego plusa (wnętrze)
//    EVE_COLOR_RGB(255, 255, 255); // Biały kolor
//    EVE_BEGIN(9); // Rysowanie prostokątów
//    // Poziomy prostokąt (pozioma część plusa)
//    EVE_VERTEX2F((x + borderThickness) * 16, (y + height / 2 - innerHeight) * 16);
//    EVE_VERTEX2F((x + width - borderThickness) * 16, (y + height / 2 + innerHeight ) * 16);
//    // Pionowy prostokąt (pionowa część plusa)
//    EVE_VERTEX2F((x + width / 2 - innerWidth) * 16, (y + borderThickness) * 16);
//    EVE_VERTEX2F((x + width / 2 + innerWidth) * 16, (y + height - borderThickness) * 16);
//    EVE_END();
//
//    // Rysowanie czerwonego plusa (obramowanie)
//    EVE_COLOR_RGB(255, 0, 0); // Czerwony kolor
//    EVE_BEGIN(9); // Rysowanie prostokątów
//        // Poziomy prostokąt (pozioma część plusa)
//    EVE_VERTEX2F(x * 16, (y + height / 2 - borderThickness * 2) * 16);
//    EVE_VERTEX2F((x + width) * 16, (y + height / 2 + borderThickness * 2) * 16);
//
//    // Pionowy prostokąt (pionowa część plusa)
//    EVE_VERTEX2F((x + width / 2 - borderThickness * 2) * 16, y * 16);
//    EVE_VERTEX2F((x + width / 2 + borderThickness * 2) * 16, (y + height) * 16);
//    EVE_END();
//
//    // Powrót koloru do domyślnego
//    EVE_COLOR_RGB(255, 255, 255);
//}
