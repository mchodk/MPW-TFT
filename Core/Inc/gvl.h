/*
 * gvl.h
 *
 *  Created on: Oct 30, 2024
 *      Author: cezary.macik
 */

#ifndef INC_GVL_H_
#define INC_GVL_H_

#define DETECT_PASSAGE_SIDE ((sideAdress) & (0x20))
#define LIFTCAN_DATA 0
#define DIAGNOSTIC_DATA 1
#define SEND_HEARTBEAT 1
#define CAN_INIT 2
#define CAN_RETRANMISSION_ATTEMPTS 5

#define BRAK_LIFTCAN 200

typedef struct{
	uint8_t flashEoEu;
	uint8_t SetOutpus;
	uint8_t displayOff;
	uint8_t buzzerRunState;
	uint8_t DataToSendCANReady;
	uint8_t CanByteSeg1;
	uint8_t CanByteSeg2;
	uint8_t CanByteSeg3;
	uint8_t CanByteSeg4;
	uint8_t OutputReady;
	uint8_t canCommandReady;
	uint8_t canCommandData;
	uint8_t displayReady;
} GvlList;

typedef struct{
	uint16_t RxAdress;
	uint16_t TxAdress;
	uint16_t DiagTxAdress;
	uint16_t DisplayAdress;
} CanAdress;

typedef enum {
	FLASH_EO_EU 		=	0x02,
	CLEAR_CALL_ACK 		=	0x05,
	SYSTEM_RESET 		=	0x07,
	DATA_READY 			=	0x08,
	DISPLAY_OFF 		=	0x0B,
	DISPLAY_ON 			=	0x0C,
	REMOTE_BUZZER_ON	=	0x11,
	REMOTE_BUZZER_OFF 	=	0x12
} commands;

typedef enum{
	ONE_BYTE = 1,
	TWO_BYTE = 2
} DLC;

typedef enum {
	CAN_SETUP_OK,
	CAN_NOT_READY
} canState;

extern CanAdress liftCanAdress;
extern GvlList global;

#endif /* INC_GVL_H_ */
