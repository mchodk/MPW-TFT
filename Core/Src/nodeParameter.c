/*
 * nodeParameter.c
 *
 *  Created on: Oct 21, 2024
 *      Author: cezary.macik
 */
#include "i2c.h"
#include "main.h"
#include "nodeParameter.h"
#include "crc.h"

nodeParam nodeParameters[IL_PAR]=
{/*	W	Adr	  Mn   Mx	*/
	{0, 0x00,  0,  0},  	// 0
	{0, 0x01,  0, 33},  	// 1 adres litcan
	{0, 0x02,  0,  1},  	// 2 strona glowna/rzeplotowa
    {0, 0x03,  1, 9},  		// 3 jasnosc wyswietlacza
	{0, 0x04,  0,  3},  	// 4 obrot
	{0, 0x05,  0,  1},  	// 5 buzzer
	{0, 0x06,  0,  10},  	// 6 gong_vol
	{0, 0x07,  0,  1},  	// 7 migajace strzalki
	{0, 0x08,  0,  2},  	// 8 język
	{0, 0x09,  0,  9},  	// 9 kolor tla
	{0, 0x0A,  0,  0},  	// 10 wolny
	{0, 0x0B,  0,  0} 		// 11 wolny
};

uint32_t readParameters (void)
{
	uint8_t value = 0;
	uint8_t paramBuffer[IL_PAR];

	for(uint8_t i = 0; i < IL_PAR; i++)
	{
		value = readEEPROM(nodeParameters[i].adres);

		if ((value >= nodeParameters[i].min) && (value <= nodeParameters[i].max))
			nodeParameters[i].wartosc = value;

		HAL_Delay(20);
	}

	for (uint8_t j = 0; j < IL_PAR; j++)
		paramBuffer[j] = nodeParameters[j].wartosc;

	return HAL_CRC_Calculate(&hcrc, (uint32_t *)paramBuffer, IL_PAR);
}

uint8_t saveParameters (void)
{
	uint8_t status = 0;
	uint8_t paramBuffer[IL_PAR];

	for(uint8_t i = 0; i < IL_PAR; i++)
	{
		status = writeEEPROM(nodeParameters[i].adres, nodeParameters[i].wartosc);
		HAL_Delay(20);
	}

	for (uint8_t j = 0; j < IL_PAR; j++)
		paramBuffer[j] = nodeParameters[j].wartosc;

	saveCRCinEEPROM(HAL_CRC_Calculate(&hcrc, (uint32_t *)paramBuffer, IL_PAR));

	if(status == HAL_OK)
		return HAL_OK;
	else
		return HAL_ERROR;
}

//uint8_t saveSingleParameters (uint8_t param, uint8_t value)
//{
//	uint8_t status;
//
//	status = writeEEPROM(nodeParameters[param].adres, nodeParameters[value].wartosc);
//	HAL_Delay(20);
//
//	if(status == HAL_OK)
//		return HAL_OK;
//	else
//		return HAL_ERROR;
//}

void saveCRCinEEPROM(uint32_t value)
{
	writeEEPROM(0x10, (uint8_t)(value & 0xFF));        // LSB
	writeEEPROM(0x11, (uint8_t)((value >> 8) & 0xFF));
	writeEEPROM(0x12, (uint8_t)((value >> 16) & 0xFF));
	writeEEPROM(0x13, (uint8_t)((value >> 24) & 0xFF)); // MSB
}

uint32_t readCRCEEPROM(void)
{
    uint32_t crcValue = 0;

    crcValue |= (uint32_t)readEEPROM(0x10);              // LSB z 0x10
    crcValue |= (uint32_t)readEEPROM(0x11) << 8;         // Drugi bajt z 0x11
    crcValue |= (uint32_t)readEEPROM(0x12) << 16;        // Trzeci bajt z 0x12
    crcValue |= (uint32_t)readEEPROM(0x13) << 24;        // MSB z 0x13

    return crcValue;
}

//void changeParameters (uint8_t menu_pos)
//{
//	switch (menu_pos)
//	{
//		/*case 3: LIFTCAN=24; if (dane_mod[3].wartosc) LIFTCAN=36;
//		case 4: LCD_com2(0x81,dane_mod[menu_poz].wartosc+130); break;
//		case 5: __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, dane_mod[5].wartosc);
//				JASNOSC=dane_mod[5].wartosc; break;
//		case 6: if (dane_mod[menu_poz].wartosc) LCD_com(0xA7); else LCD_com(0xA6); break;
//		case 7: LCD_clear();
//				if ((dane_mod[menu_poz].wartosc==1)||(dane_mod[menu_poz].wartosc==3)) LCD_com2(0xC0, 0x04); else LCD_com2(0xC0, 0x02);
//				display_ready=1; break;
//		case 9: display_ready=1; break;
//		case 11: display_ready=1; jezyk=dane_mod[11].wartosc; break;*/
//	}
//}
