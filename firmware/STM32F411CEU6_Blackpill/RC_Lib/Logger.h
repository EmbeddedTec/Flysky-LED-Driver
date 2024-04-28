/*
 * Logger.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Felix
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

extern char Log_msg[255];
extern uint32_t Log_msg_length;

void Log(char *message);
void Logl(char *message, uint32_t length);

#endif // LOGGER_H
