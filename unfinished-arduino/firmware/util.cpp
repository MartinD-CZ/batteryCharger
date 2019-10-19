#include <avr\io.h>
#include "util.h"
#include "defines.h"
#include <avr\interrupt.h>
#include <stdio.h>
#include "Arduino.h"

volatile uint16_t adc_result[ADC_CHANNELS];
volatile uint8_t adc_index = 0;

uint16_t max_voltage;
uint16_t max_current;
bool charge_enabled;

volatile uint8_t timer2_counter, tSec, tMin, tHrs;
volatile uint16_t tRunMin;

void init_pwm(void)
{
	//reset timer 0 registers
	TCCR0A = 0x00;
	TCCR0B = 0x00;

	//setup pin D5 and D6 as output
	DDRD |= (1 << DDD5) | (1 << DDD6);

	//setup timer 0 for fast 8 bit PWM @ 62.5 kHz
	MODE_NONE;
	TCCR0B = (1 << CS00);

	//0 % duty cycle
	CHARGE_PWM_DUTY = 0;
	DISCHRG_PWM_DUTY = 0;
}

void init_adc(void)
{
	//AVcc as reference, with a cap at AREF
	ADMUX = (1 << REFS0);
	
	//prescaler of 128, interrupt enable, start the ADC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE);

	sei();
}

ISR(ADC_vect)
{
	adc_result[adc_index++] = ADC;

	if (adc_index == ADC_CHANNELS)
		adc_index = 0;

	switch (adc_index)
	{
	case ADC_CH_Vbat:
		ADMUX = (1 << REFS0);
		break;
	case ADC_CH_Ibat:
		ADMUX = (1 << REFS0) | (1 << MUX0);
		break;
	default:
		ADMUX = (1 << REFS0);
		break;
	}

	ADCSRA |= (1 << ADSC);
}

uint16_t read_data(uint8_t channel)
{
	switch (channel)
	{
	case ADC_CH_Vbat:
		return (adc_result[channel] * ADC_RESOLUTION * 4.05);
	case ADC_CH_Ibat:
		return (adc_result[channel] * ADC_RESOLUTION * 10.0 / 15.0);
	default:
		return (adc_result[channel] * ADC_RESOLUTION * 4);
	}
}

void init_timer2(void)
{
	//timer 2 is used to periodically adjust the PWM
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	TIMSK2 = (1 << TOIE2);
}

ISR(TIMER2_OVF_vect)
{
	//overflows 61.2 times a second
	if (charge_enabled)
	{
		if ((read_data(ADC_CH_Vbat) < max_voltage) & (read_data(ADC_CH_Ibat) < max_current) & (CHARGE_PWM_DUTY < 255))
			CHARGE_PWM_DUTY++;
		else if (CHARGE_PWM_DUTY > 0)
			CHARGE_PWM_DUTY--;
	}

	timer2_counter++;
	if (timer2_counter >= 61)
	{
		timer2_counter = 0;
		tSec++;
		if (tSec >= 60)
		{
			tSec = 0;
			tMin++;
			tRunMin++;
			if (tMin >= 60)
			{
				tMin = 0;
				tHrs++;
			}
		}
	}
}

void init_gpio(void)
{
	pinMode(2, INPUT_PULLUP);
	pinMode(3, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);

	PCICR = (1 << PCIE2);						//pin change interrupt for encoder
	PCMSK2 = (1 << PCINT20) | (1 << PCINT18);	//enable PCINT for input A and the button
}