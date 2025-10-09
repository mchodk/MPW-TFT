/*
 * sendString.c
 *
 *  Created on: 08.04.2024
 *      Author: cezary.macik
 */

#include "sendString.h"


Status_t sendUartString(const char* format, ...)
{
//    char buffer[MAX_BUFFER] = {0};
//
//    va_list args; //z biblioteki arg.h z racji tego, ze funkcje moze przyjad nieokreslona ilosc argumentow
//
//    va_start(args, format); //z biblioteki arg.h
//    vsnprintf(buffer, sizeof(buffer), format, args); //z biblioteki arg.h
//    va_end(args); //z biblioteki arg.h
//
//    if(HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY) != HAL_OK)
//    	return UART_ERROR;
//    else
//    	return UART_OK;
}
