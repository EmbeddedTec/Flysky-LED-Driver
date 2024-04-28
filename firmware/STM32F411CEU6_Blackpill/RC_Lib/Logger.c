/*
 * Logger.c
 *
 *  Created on: Apr 23, 2024
 *      Author: Felix
 */


#include "Logger.h"
#include "usart.h"
#include "string.h"

char Log_msg[255];
uint32_t Log_msg_length;


void Log(char *message)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

void Logl(char *message, uint32_t length)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)message, length, HAL_MAX_DELAY);
}
