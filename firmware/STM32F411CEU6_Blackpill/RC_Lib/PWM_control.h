/*
 * PWM_mapping.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Felix
 */


#ifndef PWM_MAPPING_H
#define PWM_MAPPING_H

#include <stdint.h>

#define PWM_IN_MIN_MS 1.0
#define PWM_IN_MAX_MS 2.5
#define PWM_OUT_MIN_PERCENT 0.0
#define PWM_OUT_MAX_PERCENT 100.0
#define TIM3_PERIOD 10000 // 1kHz


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



void UpdatePulseWidth(RC_CHANNEL_TYPE channel, uint32_t pw);
uint32_t getMode(RC_CHANNEL_TYPE channel);
uint32_t getLast(RC_CHANNEL_TYPE channel);

#endif // PWM_MAPPING_H

