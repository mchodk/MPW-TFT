/*
 * buzzer.h
 *
 *  Created on: Nov 12, 2024
 *      Author: cezary.macik
 */

#ifndef INC_BUZZER_H_BAK_
#define INC_BUZZER_H_BAK_

#include "i2c.h"
#include "nodeParameter.h"
#include "gvl.h"

#define BUZZER_TIME 8
#define MAX_VOL 255
#define BUZZER_SOUND ((0x54 << 8) | 0x42)
#define GONG 0x49
#define GONG_SOUND ((0x6C << 8) | GONG)

void setBuzzer(void);
void chceckBuzzer(uint8_t value, uint8_t status);
void checkBuzzerSound(void);
void checkGong(void);

//extern uint16_t buzzer_delay, buzzer_count, buzzer_status, buzzer_start;

#endif /* INC_BUZZER_H_BAK_ */
