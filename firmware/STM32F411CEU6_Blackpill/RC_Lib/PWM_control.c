#include "PWM_control.h"
#include "tim.h"

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


static uint32_t Last_Ch3;
static uint32_t Last_Ch4;

static RC_MODE_CH3_T mode_ch3 = RC_MODE3_OFF;
static RC_MODE_CH4_T mode_ch4 = RC_MODE4_OFF;


uint8_t getModeFromPulsewidthinRange(RC_MODE_RANGE_T range, uint32_t pw);



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

	switch (channel) {
	case RC_CH3:
		mode_ch3 = getModeFromPulsewidthinRange(ranges_CH3, pw);
		Last_Ch3 = pw;
		break;
	case RC_CH4:
		mode_ch4 = getModeFromPulsewidthinRange(ranges_CH4, pw);
		Last_Ch4 = pw;
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
