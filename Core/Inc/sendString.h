/*
 * sendString.h
 *
 *  Created on: 08.04.2024
 *      Author: cezary.macik
 */

#ifndef SENDSTRING_H_
#define SENDSTRING_H_

#include "stdarg.h"
#include "usart.h"
#include "string.h"

#define MAX_BUFFER 256
#define BUFFER_SIZE 100


//UART_HandleTypeDef huart1;
//HAL_StatusTypeDef status;

typedef enum{
	UART_OK,
	UART_ERROR
}Status_t;

//typedef enum {
//    PL = 1,
//    EN,
//    LT,
//    SZ,
//    IT,
//    FR,
//    RU,
//    UA
//} VoiceLanguage_t;

Status_t sendUartString(const char* format, ...);

#endif /* SENDSTRING_H_ */
