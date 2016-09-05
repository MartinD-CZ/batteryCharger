/*
 * AVR MultiChemistry charger
 * file with NiMH/NiCd charging algorithm
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 

#include <avr/io.h>
#include "NiMH.h"
#include "..\header.h"
#include "..\display\menu.h"
#include "..\display\5110.h"
#include "..\uart\uart.h"
#include "..\adc.h"
#include <inttypes.h>
extern LCD_5110 lcd;
extern volatile bool secondFlag;

void chargeNiMH(float maxCurrent, uint8_t numberOfCells, uint8_t maxHrs)
{
	float maxVoltage = numberOfCells * MAX_CELL_VOLTAGE;

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
	bool charging = true;
	uint32_t miliAmpSecs = 0, miliWattSecs = 0;
	uint8_t tHrs = 0, tMin = 0, tSec = 0, minuteCounter = 0;
	float batteryCurrent, batteryVoltage, batteryTemperature, lastMinuteVoltage;
	uint8_t termination = RUNNING;

	lastMinuteVoltage = measureBatteryVoltage();
	PWM_SUPPLY_MODE;
	PWM_START;
	TIMER1_START;

	while (charging)
	{
		//constant current regulation routine
		batteryCurrent = measureChargingCurrent();
		if ((batteryCurrent < (maxCurrent * 0.95)) && (SUPPLY_PWM < 0xFF))
		{
			SUPPLY_PWM++;
		}
		else if ((batteryCurrent > (maxCurrent * 1.05) && (SUPPLY_PWM > 0x00)))
		{
			SUPPLY_PWM--;
		}
		
		//do this every second
		if (secondFlag)
		{
			secondFlag = false;
			minuteCounter++;
			YEL_LED_TOGGLE;

			//calculate time
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

			lcd.lcd_goto_xy_exact(0, 0);
			lcd.lcd_string_format("NiCd/NiMH CHRGI: %4.2f/%4.2f AV:%5.2f/%5.2fV%02d:%02d:%02d %3d C", batteryCurrent, maxCurrent, batteryVoltage, maxVoltage, tHrs, tMin, tSec, batteryTemperature);
			
			#ifdef UART
				uart_sendStrFormatted("%5.2f;%4.2f;%02d:%02d:%02d;%03d;%3d;\n", batteryVoltage, batteryCurrent, tHrs, tMin, tSec, SUPPLY_PWM, batteryTemperature);
			#endif

			//Termination methods (overvoltage, user aborting operation, -dV/dT method, time, overheating)
			//***********************************************
			if (batteryVoltage > maxVoltage)
			{
				charging = false;
				termination = OVERVOLTAGE;
			}

			if (BTN_MIDDLE_PRESSED)
			{
				charging = false;
				termination = USER;
			}

			if (minuteCounter > 59)
			{
				minuteCounter = 0;
				if ((lastMinuteVoltage - batteryVoltage) > (NEG_dV * numberOfCells))
				{
					charging = false;
					termination = deltaV;
				}
				lastMinuteVoltage = batteryVoltage;
			}

			if (tHrs >= maxHrs)
			{
				charging = false;
				termination = TIME;
			}
			
			if (batteryTemperature > MAX_TEMPERATURE)
			{
				charging = false;
				termination = OVERTEMPERATURE;
			}
		}

	}

	//evaluate the charging process
	PWM_STOP;
	PWM_RESET;
	TIMER1_STOP;
	YEL_LED_OFF;
	RED_LED_ON;

	lcd.lcd_clear();
	lcd.lcd_goto_xy_exact(0, 0);
	lcd.lcd_string_format("CHARGING\nFINISHED\nE: %5d mAh\nE: %5d mWh\nT: %02d:%02d:%02d", (int)(miliAmpSecs / 3600), (int)(miliWattSecs / 3600), tHrs, tMin, tSec);
		
	switch (termination)
	{
		case 1:
			lcd.lcd_str("OVERVOLTAGE");
			break;
		case 2:
			lcd.lcd_str("BATT TOO WARM!");
			break;
		case 3:
			lcd.lcd_str("BTN PRESSED");
			break;
		case 4:
			lcd.lcd_str("-DELTA V");
			break;
		case 5:
			lcd.lcd_str("TIME OUT");
			break;
	}

	while (!BTN_MIDDLE_PRESSED);
	while (BTN_MIDDLE_PRESSED);

	RED_LED_OFF;
	

	#ifdef UART
		uart_sendStr("CHARGE ENDED\n");
		uart_sendStrFormatted("Vmax: %5.2f V, Imax: %4.2f V\n", maxVoltage, maxCurrent);
	#endif


}
