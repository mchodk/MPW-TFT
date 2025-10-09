/*
 * buzzer.c
 *
 *  Created on: Nov 12, 2024
 *      Author: cezary.macik
 */
#include <buzzer.h>
#include <EVE.h>

void setBuzzer(void)
{
	uint8_t tmp = 0;

	if (global.buzzerRunState == 1)
		tmp = 1;
	else if (global.buzzerRunState == 2)
		tmp = 0;

	if ((!tmp && nodeParameters[BUZZER].wartosc) || (tmp && !nodeParameters[BUZZER].wartosc))
	{
		nodeParameters[BUZZER].wartosc = tmp;
		writeEEPROM(nodeParameters[BUZZER].adres, nodeParameters[BUZZER].wartosc);
	}

	global.buzzerRunState = 0;
}

void checkGong(void)
{
	HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
	HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
   	HAL_MemWrite16(EVE_REG_SOUND, GONG_SOUND);
    HAL_MemWrite8(EVE_REG_PLAY, 1);
    while(HAL_MemRead8(EVE_REG_PLAY));
    HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
}

void checkBuzzerSound(void)
{
	HAL_MemWrite8(EVE_REG_GPIOX_DIR, 0x8);
	HAL_MemWrite8(EVE_REG_GPIOX, 0x8);
	HAL_MemWrite8 (EVE_REG_VOL_SOUND, MAX_VOL);
   	HAL_MemWrite16(EVE_REG_SOUND, BUZZER_SOUND);
    HAL_MemWrite8(EVE_REG_PLAY, 1);
    while(HAL_MemRead8(EVE_REG_PLAY));
    HAL_MemWrite8(EVE_REG_GPIOX, 0x0);
}
