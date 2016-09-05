/*
 * AVR MultiChemistry charger
 * file with constant C-V charging algorithm
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 

#include <avr/io.h>
#include "constantCV.h"
#include "..\header.h"
#include "..\display\menu.h"
#include "..\display\5110.h"
#include "..\uart\uart.h"
#include "..\adc.h"
#include <inttypes.h>
extern LCD_5110 lcd;
extern volatile bool secondFlag;

void constantCVCharge(float maxCurrent, float maxVoltage)
{
	//review all info on the LCD
	lcd.lcd_clear();
	lcd.lcd_goto_xy_exact(0, 0);
	lcd.lcd_string_format("REVIEW:\nIchrg:  %4.2f AVmax:  %5.2f VPlugin batteryand press\nmiddle button", maxCurrent, maxVoltage);
	
	#ifdef UART
		uart_sendStr("STARTING CC-CV CHARGE\n");
		uart_sendStrFormatted("Vmax: %5.2f V, Imax: %4.2f V\n", maxVoltage, maxCurrent);
		uart_sendStr("V;I;tHrs:tMin:tSec;PWM;temp\n");
	#endif
	
	while (!BTN_MIDDLE_PRESSED);
	while (BTN_MIDDLE_PRESSED);
	lcd.lcd_clear();

	//variables
	bool constantCurrent, charging = true;
	uint32_t miliAmpSecs = 0, miliWattSecs = 0;
	uint8_t tHrs = 0, tMin = 0, tSec = 0;
	float batteryCurrent, batteryVoltage, batteryTemperature;
	uint8_t termination = RUNNING;
	if (maxCurrent == 0)		//if set current is 0, do not regulate current at all
	{
		constantCurrent = false;
		maxCurrent = 3.00;
	}
	else
	{
		constantCurrent = true;
	}

	//charging routine
	PWM_SUPPLY_MODE;
	PWM_START;
	TIMER1_START;

	while (charging)
	{
		//voltage/current regulation subroutine
		if (constantCurrent)	//if we are in the constant current mode
		{
			batteryCurrent = measureChargingCurrent();
			if ((batteryCurrent < (maxCurrent * 0.95)) && (SUPPLY_PWM < 0xFF))
			{
				SUPPLY_PWM++;
			}
			else if ((batteryCurrent > (maxCurrent * 1.05) && (SUPPLY_PWM > 0x00)))
			{
				SUPPLY_PWM--;
			}
		}
		else          //if we are in constant voltage mode
		{
			batteryVoltage = measureBatteryVoltage();
			if ((batteryVoltage < (maxVoltage - 0.05)) && (SUPPLY_PWM < 0xFF))
			{
				SUPPLY_PWM++;
			}
			else if ((batteryVoltage > maxVoltage && (SUPPLY_PWM > 0x00)))
			{
				SUPPLY_PWM--;
			}
		}

		//do this every second
		if (secondFlag)
		{
			secondFlag = false;
			YEL_LED_TOGGLE;

			//calculating time
			tSec++;
			if (tSec > 59)
			{
				tSec = 0;
				tMin++;
			}
			if (tMin > 59)
			{
				tMin = 0;
				tHrs++;
			}

			//battery measurement routine
			batteryVoltage = measureBatteryVoltage();
			batteryCurrent = measureChargingCurrent();
			batteryTemperature = measureBatteryTemperature();
			miliAmpSecs += (batteryCurrent * 1000);
			miliWattSecs += (batteryVoltage * batteryCurrent * 1000);

			lcd.lcd_clear();
			lcd.lcd_goto_xy_exact(0, 0);
			lcd.lcd_string_format("CHARGING:\nI: %4.2f/%4.2f AV:%5.2f/%5.2fVE: %5d mAh  %02d:%02d:%02d %3d C", batteryCurrent, maxCurrent, batteryVoltage, maxVoltage, (int)(miliAmpSecs / 3600), tHrs, tMin, tSec, batteryTemperature);

			#ifdef UART
				uart_sendStrFormatted("%5.2f;%4.2f;%02d:%02d:%02d;%03d;%3d;\n", batteryVoltage, batteryCurrent, tHrs, tMin, tSec, SUPPLY_PWM, batteryTemperature);
			#endif 
			

			//Termination methods
			//********************************************
			if (constantCurrent)
			{
				lcd.lcd_str("CONST C");
				if (batteryVoltage > maxVoltage)
				{
					constantCurrent = false;
				}
			}
			else
			{
				lcd.lcd_str("CONST V");
				if (batteryCurrent < (maxCurrent * 0.1))
				{
					charging = false;
					termination = REGULAR;
				}
				else if (batteryCurrent > maxCurrent)
				{
					constantCurrent = true;
				}
			}

			if (BTN_MIDDLE_PRESSED)
			{
				charging = false;
				termination = USER;
			}

			if (batteryTemperature > MAX_TEMPERATURE)
			{
				charging = false;
				termination = OVERTEMPERATURE;
			}
		}

	}

	//evaluate the charging proccess
	PWM_STOP;
	PWM_RESET;
	TIMER1_STOP;
	YEL_LED_OFF;
	RED_LED_ON;

	lcd.lcd_clear();
	lcd.lcd_goto_xy_exact(0, 0);
	lcd.lcd_string_format("CHARGING\nFINISHED\nE: %5d mAh\nE: %5d mWh\nT: %02d:%02d:%02d   ", (int)(miliAmpSecs / 3600), (int)(miliWattSecs / 3600), tHrs, tMin, tSec);
	switch (termination)
	{
		case 1:
		lcd.lcd_str("CHRG SUCCESS");
		break;
		case 2:
		lcd.lcd_str("BATT TOO WARM!");
		break;
		case 3:
		lcd.lcd_str("BTN PRESSED");
		break;
	}

	while (BTN_MIDDLE_PRESSED);
	while (!BTN_MIDDLE_PRESSED);

	RED_LED_OFF;
	

	#ifdef UART
		uart_sendStr("CHARGE ENDED\n");
		uart_sendStrFormatted("Vmax: %5.2f V, Imax: %4.2f A\n", maxVoltage, maxCurrent);
	#endif

}