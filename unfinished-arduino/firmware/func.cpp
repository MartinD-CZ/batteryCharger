#include <avr\io.h>
#include "func.h"
#include "defines.h"
#include "Arduino.h"
#include "util.h"
#include "display.h"

extern uint16_t max_voltage;
extern uint16_t max_current;
extern bool charge_enabled;

extern bool btn_pressed;

extern volatile uint8_t tSec, tMin, tHrs;

char buffer[4][16];

void func_chargeLi(uint16_t tgtVolt, uint16_t tgtAmp, uint16_t cutOffI)
{
	max_voltage = tgtVolt;
	max_current = tgtAmp;
	charge_enabled = true;
	MODE_CHARGE;
	tSec = 0; tMin = 0; tHrs = 0;

	btn_pressed = false;
	while (!btn_pressed)
	{
		func_printf(0, "V:%4d/%4d mV", read_data(ADC_CH_Vbat), tgtVolt);
		func_printf(1, "I:%4d/%4d mA", read_data(ADC_CH_Ibat), tgtAmp);
		func_printf(2, "PWM: %3d", CHARGE_PWM_DUTY);
		func_printf(3, "CV    %02d:%02d:%02d", tHrs, tMin, tSec);
		display_drawFunction("LiIon Charge", buffer[0], buffer[1], buffer[2], buffer[3]);
		_delay_ms(500);
	}
	MODE_NONE;
	_delay_ms(100);
	btn_pressed = false;
}

void func_printf(uint8_t bufIndex, char *szFormat, ...)
{
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(buffer[bufIndex], 15, szFormat, pArgs);
	va_end(pArgs);
}

void func_discharge(uint16_t current, uint16_t minVolt)
{
	DISCHRG_PWM_DUTY = (current * 2.0 * 0xFF) / VIN;
	MODE_DISCHARGE;
	tSec = 0; tMin = 0; tHrs = 0;

	uint16_t volt = 0xFFFF;
	_delay_ms(100);
	btn_pressed = false;
	while (!btn_pressed)
	{
		volt = read_data(ADC_CH_Vbat);
		func_printf(0, "V:%4d/%4d mV", volt, minVolt);
		func_printf(1, "I: %4d mA", current);
		func_printf(2, "PWM: %3d", DISCHRG_PWM_DUTY);		//ToDo: mAh
		func_printf(3, "      %02d:%02d:%02d", tHrs, tMin, tSec);
		display_drawFunction("Discharge", buffer[0], buffer[1], buffer[2], buffer[3]);
		_delay_ms(500);
	}
	_delay_ms(100);
	btn_pressed = false;
	DISCHRG_PWM_DUTY = 0;
	MODE_NONE;
}