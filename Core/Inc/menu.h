/*
 * menu.h
 *
 *  Created on: Oct 24, 2024
 *      Author: cezary.macik
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "main.h"
#include "nodeParameter.h"
#include "EVE.h"

#define DEF_PAR 20
#define _LIFTCAN_ADRESS 40
#define _SIDE 60
#define _BRIGHTTNESS 80
#define _ROTATE 100
#define _BUZZER 120
#define _GONG_VOL 140
#define _FLASHING_ARROWS 160
#define _CODE 180
#define _COLOR 200

#define COPY_DATA 220
#define _BACK_B 240

#define SAVE_PARAMETER 220
//#define SAVE_PARAMETER 260
#define EXIT 240
//#define EXIT 280
#define COLOR_VALUE 10

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Color;

void tftDispMenu (uint8_t status);
void tftMatrixTest (void);

extern volatile uint16_t menuPosSel;
extern Color colorRgbCode[COLOR_VALUE];

#endif /* INC_MENU_H_ */
