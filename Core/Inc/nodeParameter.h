/*
 * nodeParameter.h
 *
 *  Created on: Oct 21, 2024
 *      Author: cezary.macik
 */
#ifndef INC_NODEPARAMETER_H_
#define INC_NODEPARAMETER_H_

#define PARAMETERS 		12
//#define BUZZER_DELAY	2000
#define MENU_TIME 		20000
#define FADE_DELAY 		20
#define RESET_DELAY 	3001
#define BUZZER_MENU 	8
#define JASNOSC_MENU 	5
#define MENU_TEXT 		11
#define IL_PAR 			12

uint32_t readParameters (void);
uint8_t saveParameters (void);
//void changeParameters (uint8_t menu_pos);
uint32_t readCRCEEPROM(void);
void saveCRCinEEPROM(uint32_t value);
//uint8_t saveSingleParameters (uint8_t param, uint8_t value);

typedef struct
{
	uint8_t wartosc;
	uint8_t adres;
	uint8_t min;
	uint8_t max;
} nodeParam;

typedef enum {
	LIFCAN_ADRESS = 1,
	SIDE,
	BRIGHTTNESS,
	ROTATE,
	BUZZER,
	GONG_VOL,
	FLASHING_ARROWS,
	LANGUAGE,
	COLOR,
} paramName;

typedef enum{
	BLACK,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	GRAY,
	FIOLET
} colors;

//extern const char* menuText[];
extern nodeParam nodeParameters[IL_PAR];
#endif /* INC_NODEPARAMETER_H_ */
