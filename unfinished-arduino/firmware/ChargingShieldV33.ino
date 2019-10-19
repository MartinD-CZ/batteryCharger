/*
 Name:		ChargingShieldV33.ino
 Created:	3/24/2019 3:47:11 PM
 Author:	Martin Danek
*/

#include "util.h"
#include "defines.h"
#include <avr\delay.h>
#include <avr\interrupt.h>
#include "display.h"
#include "func.h"

const char* mainMenu[] = {"Chemistry", "Li-Ion/Pol", "NiMH/NiCd", "SLA/Pb-based", "About"};
const char* liIonMenu[] = {"Operation", "Charge", "Discharge", "Capacity Test", "Back"};

void setup()
{
	Serial.begin(115200);
	
	cli();
	init_pwm();
	MODE_NONE;

	init_gpio();

	init_adc();
	ADC_START;

	init_timer2();
	sei();

	Serial.println("Charging shield V3.3 ready!");

	display_init();
}

void loop()
{
	switch (display_drawMenu(mainMenu))
	{
	case 0:		//Li-Ion
	{
		switch (display_drawMenu(liIonMenu))
		{
		case 0:		//Li-Ion: Charge
		{
			uint16_t tgtVolt = display_drawNumMenu("MAX VOLTAGE", "mV", 4200, 1000, 10000, 50);
			uint16_t tgtAmp = display_drawNumMenu("MAX CURRENT", "mA", 1000, 50, 2000, 100);
			uint16_t minAmp = display_drawNumMenu("CUTOFF CURR", "mA", 100, 50, 2000, 100);
			func_chargeLi(tgtVolt, tgtAmp, minAmp);
			break;
		}
		case 1:		//Li-Ion: Discharge
		{
			uint16_t tgtAmp = display_drawNumMenu("CURRENT", "mA", 1000, 100, 1500, 100);
			uint16_t minVolt = display_drawNumMenu("MIN. VOLT", "mV", 2500, 500, 10000, 100);
			func_discharge(tgtAmp, minVolt);
			break;
		}
		case 2:		//Li-Ion: Capacity test
		{
			display_msg("Error 0.2", "Not Available");
			break;
		}
		default:
			return;
		}
		break;
	}
	case 1:		//NiMH/NiCd
	{
		display_msg("Error 1", "Not Available");
		break;
	}
	case 2:		//SLA/Pb-based
	{
		display_msg("Error 2", "Not Available");
		break;
	}
	case 3:		//settings
	{
		display_msg("Error 3", "Not Available");
		break;
	}
	default:
	{
		display_msg("Error 4", "Out of range");
		break;
	}

	}
	
	
	/*uint16_t volts = read_data(ADC_CH_Vbat);
	uint16_t amps = read_data(ADC_CH_Ibat);
	uint32_t watts = volts * amps / 1000;
	
	Serial.print("Vbat: ");
	Serial.print(volts);
	Serial.print(" mV; Ibat: ");
	Serial.print(amps);
	Serial.print(" mA; PWM: ");
	Serial.print(CHARGE_PWM_DUTY);
	//Serial.print(" mV; PWM: ");
	//.print(DISCHRG_PWM_DUTY);
	Serial.print("; P: ");
	Serial.print(watts);
	Serial.println(" W");
	_delay_ms(1000);*/
}
