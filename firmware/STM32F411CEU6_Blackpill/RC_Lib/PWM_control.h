/*
 * PWM_mapping.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Felix
 */


#ifndef PWM_MAPPING_H
#define PWM_MAPPING_H

#include <stdint.h>
#include "switches.h"
#include "Logger.h"

#define PWM_IN_MIN_MS 1.0
#define PWM_IN_MAX_MS 2.5
#define PWM_OUT_MIN_PERCENT 0.0
#define PWM_OUT_MAX_PERCENT 100.0
#define TIM3_PERIOD 10000 // 1kHz

#define PWM_MIN_VAL 0
#define PWM_MAX_VAL 1000

typedef enum{
	RC_MODE3_OFF = 0,
	RC_MODE3_0 = 1,
	RC_MODE3_1 = 2,
	RC_MODE3_2 = 3
}RC_MODE_CH3_T;

typedef enum{
	RC_MODE4_OFF = 0,
	RC_MODE4_0 = 1,
	RC_MODE4_1 = 2
}RC_MODE_CH4_T;


typedef struct{
	uint32_t lower_bound;
	uint32_t upper_bound;
	uint8_t mode;
}RC_MODE_RANGE_ARRAY_T;

typedef struct{
	const RC_MODE_RANGE_ARRAY_T* array;
	uint8_t array_size;
	uint8_t default_mode;
}RC_MODE_RANGE_T;



typedef enum{
	RC_CH3 = 0,
    RC_CH4 = 1,
}RC_CHANNEL_TYPE;


typedef enum{
	CMD_NONE = 0,
	CMD_TURN_OFF = 1,
	CMD_FULL_ON = 2,
	CMD_HALF_ON = 3
}RC_COMMANDS_E;

void PWM_Control_Init();
void PWM_Control_DeInit();


void UpdatePulseWidth(RC_CHANNEL_TYPE channel, uint32_t pw);
uint32_t getMode(RC_CHANNEL_TYPE channel);
uint32_t getLast(RC_CHANNEL_TYPE channel);
void PWM_Control_Process();

#endif // PWM_MAPPING_H

