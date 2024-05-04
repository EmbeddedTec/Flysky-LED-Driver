#include "PWM_control.h"
#include "tim.h"
#include <stdio.h>
#include "queue.h"
static const RC_MODE_RANGE_ARRAY_T ranges_array_CH3[] =
{
		{   0,		 300, 	RC_MODE3_OFF},
		{ 950,		1050, 	RC_MODE3_0},
		{1450,		1550, 	RC_MODE3_1},
		{1950,		2050, 	RC_MODE3_2},
};

static const RC_MODE_RANGE_T ranges_CH3= {
		ranges_array_CH3,4,RC_MODE3_OFF
};


static const RC_MODE_RANGE_ARRAY_T ranges_array_CH4[] =
{
		{   0,		 300, 	RC_MODE4_OFF},
		{ 950,		1050, 	RC_MODE4_0},
		{1950,		2050, 	RC_MODE4_1},
};

static const RC_MODE_RANGE_T ranges_CH4= {
		ranges_array_CH4 , 3, RC_MODE4_OFF
};

static uint8_t run = 0;

static uint32_t Last_Ch3;
static uint32_t Last_Ch4;

static volatile RC_MODE_CH3_T mode_ch3 = RC_MODE3_OFF;
static volatile RC_MODE_CH4_T mode_ch4 = RC_MODE4_OFF;

static volatile uint32_t led_pwm = 0;

static Queue commandQueue;


uint8_t getModeFromPulsewidthinRange(RC_MODE_RANGE_T range, uint32_t pw);
void process_Mode();



void PWM_Control_Init()
{
	initializeQueue(&commandQueue);

	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	// Start PWM input on TIM2_CH2
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

	led_pwm = 0;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	run = 1;

}
void PWM_Control_DeInit()
{
	run = 0;
}



uint8_t getModeFromPulsewidthinRange(RC_MODE_RANGE_T range, uint32_t pw)
{
	int i;
	for(i = 0; i< range.array_size ; i++)
	{
		if(range.array[i].lower_bound < pw && range.array[i].upper_bound > pw)
		{
			return range.array[i].mode;
		}
	}

	//When Range was not found, first entry defines default state
	return range.default_mode;
}




void UpdatePulseWidth(RC_CHANNEL_TYPE channel, uint32_t pw)
{
	static RC_MODE_CH3_T prev_mode_ch3 = RC_MODE3_OFF;
	static RC_MODE_CH4_T prev_mode_ch4 = RC_MODE4_OFF;

	switch (channel) {
	case RC_CH3:
		Last_Ch3 = pw;
		mode_ch3 = getModeFromPulsewidthinRange(ranges_CH3, pw);

		if(mode_ch3 != prev_mode_ch3)
		{
			process_Mode();
			prev_mode_ch3 = mode_ch3;
		}
		break;
	case RC_CH4:
		Last_Ch4 = pw;
		mode_ch4 = getModeFromPulsewidthinRange(ranges_CH4, pw);
		if(mode_ch4 != prev_mode_ch4)
		{
			process_Mode();
			prev_mode_ch4 = mode_ch4;
		}
		break;
	default:
		break;
	}
}



uint32_t getMode(RC_CHANNEL_TYPE channel)
{
	uint32_t retVal = 0;
	if(channel == RC_CH3)
	{
		retVal = mode_ch3;
	}
	else
	{
		if(channel == RC_CH4)
		{
			retVal = mode_ch4;
		}
	}
	return retVal;

}

uint32_t getLast(RC_CHANNEL_TYPE channel)
{
	uint32_t retVal = 0;
	if(channel == RC_CH3)
	{
		retVal = Last_Ch3;
	}
	else
	{
		if(channel == RC_CH4)
		{
			retVal = Last_Ch4;
		}
	}
	return retVal;

}





void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	static uint32_t edge1_time_CH1 = 0, edge2_time_CH1 = 0;
	static uint32_t edge1_time_CH2 = 0, edge2_time_CH2 = 0;

	if (htim->Instance == TIM2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if (htim->Instance->CCER & TIM_CCER_CC1P)
			{
				/* Falling edge - end of pulse */
				edge2_time_CH1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

				/* Calculate the pulse width by subtracting the 1st edge time from the 2nd edge time */
				uint32_t pulse_width_CH1 = edge2_time_CH1 - edge1_time_CH1;
				UpdatePulseWidth(RC_CH3, pulse_width_CH1);
				/* Switch back to rising edge detection */
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			else
			{
				/* Rising edge - start of pulse */
				edge1_time_CH1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

				/* Switch to falling edge detection */
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}
		}


		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if (htim->Instance->CCER & TIM_CCER_CC2P)
			{
				/* Falling edge - end of pulse */
				edge2_time_CH2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

				/* Calculate the pulse width by subtracting the 1st edge time from the 2nd edge time */
				uint32_t pulse_width_CH2 = edge2_time_CH2 - edge1_time_CH2;
				UpdatePulseWidth(RC_CH4, pulse_width_CH2);
				/* Switch back to rising edge detection */
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			else
			{
				/* Rising edge - start of pulse */
				edge1_time_CH2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

				/* Switch to falling edge detection */
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
			}
		}
	}
}

void LED_Off()
{
	led_pwm = 0;
	//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void LED_FullOn()
{
	led_pwm = 1000;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);

	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void LED_HalfOn()
{
	led_pwm = 500;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);

	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}


#define DIM_DELAY 1
#define INC_VAL 4
void DimTo(uint32_t value)
{
	//Limit PWM value to PWM bounds
	if(value < PWM_MIN_VAL)
	{
		value = PWM_MIN_VAL;
	}
	else
	{
		if(value > PWM_MAX_VAL)
		{
			value = PWM_MAX_VAL;
		}
	}

	if(value == led_pwm)
	{
		//do nothing
		Log_msg_length = sprintf(Log_msg, "Will stay on : %lu \r\n ",led_pwm);
		Logl(Log_msg, Log_msg_length);
	}
	else
	{
		if (value > led_pwm)
		{
			Log_msg_length = sprintf(Log_msg, "Dimming up from  %lu to %lu  \r\n",led_pwm, value);
			Logl(Log_msg, Log_msg_length);
			while(value > led_pwm)
			{
				if(led_pwm < PWM_MAX_VAL-INC_VAL)
				{
					led_pwm = led_pwm + INC_VAL;
				}
				else
				{
					led_pwm = PWM_MAX_VAL;
				}

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);
				HAL_Delay(DIM_DELAY);
			}
		}
		else // value < led_pwm
		{
			Log_msg_length = sprintf(Log_msg, "Dimming down from  %lu to %lu  \r\n",led_pwm, value);
			Logl(Log_msg, Log_msg_length);

			while(value < led_pwm)
			{
				if(led_pwm > PWM_MIN_VAL+INC_VAL)
				{
					led_pwm = led_pwm - INC_VAL;
				}
				else
				{
					led_pwm = PWM_MIN_VAL;
				}

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);
				HAL_Delay(DIM_DELAY);
			}
		}

	}
}


void Log_PWM()
{
#ifdef DEBUG_MSG
	Log_msg_length = sprintf(Log_msg, "Mode: CH3: %lu  ",getMode(RC_CH3));
	Logl(Log_msg, Log_msg_length);
	Log_msg_length = sprintf(Log_msg, "CH4: %lu\r\n",getMode(RC_CH4));
	Logl(Log_msg, Log_msg_length);

	Log_msg_length = sprintf(Log_msg, "Last: CH3: %lu  ",getLast(RC_CH3));
	Logl(Log_msg, Log_msg_length);
	Log_msg_length = sprintf(Log_msg, "CH4: %lu\r\n",getLast(RC_CH4));
	Logl(Log_msg, Log_msg_length);
#endif  // DEBUG_MSG
}


void PWM_Control_Process()
{
	RC_COMMANDS_E command = CMD_NONE;

	if(!isEmpty(&commandQueue))
	{
		command = (RC_COMMANDS_E) dequeue(&commandQueue);

		switch (command) {
		case CMD_TURN_OFF:
			DimTo(0);
			break;
		case CMD_HALF_ON:
			DimTo(250);
			break;
		case CMD_FULL_ON:
			DimTo(1000);
			break;
		default:
			//unknown command
			//something went wrong. reset queue and turn off led
			initializeQueue(&commandQueue);
			LED_Off();
			break;
		}

	}
}

void reset()
{
	initializeQueue(&commandQueue);

	led_pwm = 0;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_pwm);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}



void mode_Dim_LED()
{
	switch (mode_ch3)
	{
	case RC_MODE3_0:

		if(!isFull(&commandQueue))
		{
			enqueue(&commandQueue, (int)CMD_TURN_OFF);
		}

		break;
	case RC_MODE3_1:

		if(!isFull(&commandQueue))
		{
			enqueue(&commandQueue, (int)CMD_HALF_ON);
		}
		break;
	case RC_MODE3_2:
		if(!isFull(&commandQueue))
		{
			enqueue(&commandQueue, (int)CMD_FULL_ON);
		}

		break;
	default:
		if(!isFull(&commandQueue))
		{
			enqueue(&commandQueue, (int)CMD_TURN_OFF);
		}
		break;
	}
}

void mode_Blink_LED()
{

}

void process_Mode(){

	switch (mode_ch4) {
		case RC_MODE4_OFF:
			reset();
			break;
		case RC_MODE4_0:
			mode_Dim_LED();
			break;
		case RC_MODE4_1:
			mode_Blink_LED();
			break;
		default:
			break;
	}








	Log_PWM();

}

uint32_t map_pulse_width(uint32_t pwm_in_pulse_width)
{
	// Convert the input pulse width from microseconds to milliseconds
	float pwm_in_ms = pwm_in_pulse_width / 1000.0;

	// Map the input pulse width from the range [PWM_IN_MIN_MS, PWM_IN_MAX_MS] to the range [PWM_OUT_MIN_PERCENT, PWM_OUT_MAX_PERCENT]
	// This is done by first normalizing the input to the range [0, 1], then scaling to the target range
	float pwm_out_percentage = ((pwm_in_ms - PWM_IN_MIN_MS) / (PWM_IN_MAX_MS - PWM_IN_MIN_MS)) * (PWM_OUT_MAX_PERCENT - PWM_OUT_MIN_PERCENT);

	// Convert the output pulse width from percentage to the timer counts for TIM3
	uint32_t pwm_out_pulse_width = pwm_out_percentage * (TIM3_PERIOD / 100);

	return pwm_out_pulse_width;
}
