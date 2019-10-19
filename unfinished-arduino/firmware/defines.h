#pragma once

#define MODE_CHARGE			TCCR0A = (1 << COM0B1) | (1 << WGM00) | (1 << WGM01)
#define MODE_DISCHARGE		TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01)
#define MODE_NONE			TCCR0A = (1 << WGM00) | (1 << WGM01)

#define CHARGE_PWM_DUTY		OCR0B
#define DISCHRG_PWM_DUTY	OCR0A

#define ADC_CHANNELS		2
#define ADC_START			ADCSRA |= (1 << ADSC)
#define ADC_CH_Vbat			0
#define ADC_CH_Ibat			1
#define VIN					5070		//in milivolts
#define ADC_RESOLUTION		(VIN/1024.0)

#define F_CPU				16000000UL
