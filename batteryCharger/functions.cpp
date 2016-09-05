/*
 * AVR MultiChemistry charger
 * file containing some auxiliary functions - discharging, constant C/V supply modes etc.
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */

#include <avr/io.h>
#include "functions.h"
#include "header.h"
#include "display\menu.h"
#include "display\5110.h"
#include "uart\uart.h"
#include "adc.h"
#include <inttypes.h>
extern LCD_5110 lcd;
extern volatile bool secondFlag;


 void discharge(float maxCurrent, float minVoltage)
 {
	 //review all info on the LCD
	 lcd.lcd_clear();
	 lcd.lcd_goto_xy_exact(0, 0);
	 lcd.lcd_string_format("REVIEW:\nIdis:   %4.2f AVmin:  %5.2f VPlugin batteryand press\nmiddle button", maxCurrent, minVoltage);
	 
	 #ifdef UART
		 uart_sendStr("STARTING DISCHARGE\n");
		 uart_sendStrFormatted("Vmin: %5.2f V, Idis: %4.2f V\n", minVoltage, maxCurrent);
		 uart_sendStr("V,I,mAh,mWh,tHrs,tMin,tSec,PWM\n");
	#endif

	while (!BTN_MIDDLE_PRESSED);
	while (BTN_MIDDLE_PRESSED);
	
	 uint32_t miliAmpSecs = 0, miliWattSecs = 0;
	 uint8_t tHrs = 0, tMin = 0, tSec = 0;
	 float batteryCurrent, batteryVoltage, startingVoltage;
	 bool cycle = true;
	 
	
	 startingVoltage = measureBatteryVoltage();

	 if ((startingVoltage * maxCurrent) > 2)
	 {
		FAN_ON;
	 }

	 PWM_LOAD_MODE;
	 PWM_START;

	 for (uint8_t i = 0; i < 255; i++)
	 {
		 batteryCurrent = measureDischargingCurrent();
		 if ((batteryCurrent < maxCurrent) && (LOAD_PWM < 0xFF))
		 {
			 LOAD_PWM++;
		 }
		 else if ((batteryCurrent > maxCurrent) && (LOAD_PWM > 0x00))
		 {
			 LOAD_PWM--;
		 }
	 }
	 TIMER1_START;

	 while (cycle)
	 {
		 //do this every seconds (we are using a timer for better accuracy
		 if (secondFlag)
		 {
			 secondFlag = false;
			 YEL_LED_TOGGLE;
			 
			 //time calculation routine
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

			 batteryCurrent = measureDischargingCurrent();
			 //load adjusting routine
			 if ((batteryCurrent < maxCurrent) && (LOAD_PWM < 0xFF))
			 {
				 LOAD_PWM++;
			 }
			 else if ((batteryCurrent > maxCurrent) && (LOAD_PWM > 0x00))
			 {
				 LOAD_PWM--;
			 }

			 //battery measurement routine
			 batteryVoltage = measureBatteryVoltage();
			 miliAmpSecs += (batteryCurrent * 1000);
			 miliWattSecs += (batteryVoltage * batteryCurrent * 1000);

			 //lcd output routine
			 //lcd.lcd_clear();
			 lcd.lcd_goto_xy_exact(0, 0);
			 lcd.lcd_string_format("DISCHARGE:\nV: %5.2f/%5.2fI: %5.3f/%5.3fE:   %5d mAhE:   %5d mWh%02d:%02d:%02d   %03d", batteryVoltage, minVoltage, batteryCurrent, maxCurrent, (int)(miliAmpSecs / 3600), (int)(miliWattSecs / 3600), tHrs, tMin, tSec, LOAD_PWM);

			 uart_sendStrFormatted("%5.2f;%4.2f;%5d;%5d;%02d:%02d:%02d;%03d\n", batteryVoltage, batteryCurrent, (int)(miliAmpSecs / 3600), (int)(miliWattSecs / 3600), tHrs, tMin, tSec, LOAD_PWM);

			 //termination - battery undervoltage or user pressing the middle button
			 if ((batteryVoltage < minVoltage) || BTN_MIDDLE_PRESSED)
			 {
				 cycle = false;
			 }


		 }


	 }

	TIMER1_STOP;
	PWM_STOP;
	PWM_RESET;
	FAN_OFF;

	YEL_LED_OFF;
	RED_LED_ON;

	lcd.lcd_clear();
	lcd.lcd_goto_xy_exact(0, 0);
	lcd.lcd_string_format("RESULTS:\nV: %5.2f-%5.2fI:      %4.2f AE:   %5d mAhE:   %5d mWht:    %02d:%02d:%02d", startingVoltage, minVoltage, maxCurrent, (int)(miliAmpSecs / 3600), (int)(miliWattSecs / 3600), tHrs, tMin, tSec);

	while (BTN_MIDDLE_PRESSED);
	while (!BTN_MIDDLE_PRESSED);		//Debouncing
	RED_LED_OFF;

 }

void constantVoltage(float voltage)
{
	while (BTN_MIDDLE_PRESSED);		//debouncing from menu

	float batteryVoltage, batteryCurrent;
	PWM_SUPPLY_MODE;
	PWM_START;

	while (!BTN_MIDDLE_PRESSED)
	{
		for (int i = 0; i < 256; i++)
		{
			batteryVoltage = measureBatteryVoltage();
			if ((batteryVoltage < (voltage - 0.05)) && (SUPPLY_PWM < 0xFF))
			{
				SUPPLY_PWM++;
			}
			else if ((batteryVoltage > (voltage + 0.05) && (SUPPLY_PWM > 0x00)))
			{
				SUPPLY_PWM--;
			}
		}

		batteryCurrent = measureChargingCurrent();
			lcd.lcd_clear();

		lcd.lcd_goto_xy(0, 0);
		lcd.lcd_string_format("Vout: %5.2f \nIout: %5.2f \nP: %5.2f W\nPWM: %03d", batteryVoltage, batteryCurrent, (batteryVoltage * batteryCurrent), SUPPLY_PWM);
	}

	PWM_STOP;
	PWM_RESET;
	TIMER1_STOP;
	while (BTN_MIDDLE_PRESSED);
	
}

void constantCurrent(float current)
{
	while (BTN_MIDDLE_PRESSED);		//debouncing from menu
	
	float batteryVoltage, batteryCurrent;
	PWM_SUPPLY_MODE;
	PWM_START;

	while (!BTN_MIDDLE_PRESSED)
	{
		for (int i = 0; i < 256; i++)
		{
			batteryCurrent = measureChargingCurrent();
			if ((batteryCurrent < (current - 0.05)) && (SUPPLY_PWM < 0xFF))
			{
				SUPPLY_PWM++;
			}
			else if ((batteryCurrent > (current + 0.05) && (SUPPLY_PWM > 0x00)))
			{
				SUPPLY_PWM--;
			}
		}

		batteryVoltage = measureBatteryVoltage();
		lcd.lcd_clear();

		lcd.lcd_goto_xy(0, 0);
		lcd.lcd_string_format("Vout: %5.2f \nIout: %5.2f \nP: %5.2f W\nPWM: %03d", batteryVoltage, batteryCurrent, (batteryVoltage * batteryCurrent), SUPPLY_PWM);
	}

	PWM_STOP;
	PWM_RESET;
	TIMER1_STOP;
	while (BTN_MIDDLE_PRESSED);
}

void constantCV(float voltage, float current)
{
	while (BTN_MIDDLE_PRESSED);		//debouncing from menu
	
	float batteryVoltage, batteryCurrent;
	bool constV = true;
	PWM_SUPPLY_MODE;
	PWM_START;

	while (!BTN_MIDDLE_PRESSED)
	{
		for (int i = 0; i < 256; i++)
		{
			batteryCurrent = measureChargingCurrent();
			batteryVoltage = measureBatteryVoltage();

			if (batteryVoltage < voltage)
			{
				constV = true;
			}
			if (batteryCurrent > current)
			{
				constV = false;
			}

			if (constV)
			{
				if ((batteryVoltage < (voltage - 0.05)) && (SUPPLY_PWM < 0xFF))
				{
					SUPPLY_PWM++;
				}
				else if ((batteryVoltage > (voltage + 0.05) && (SUPPLY_PWM > 0x00)))
				{
					SUPPLY_PWM--;
				}
			}
			else
			{
				if ((batteryCurrent < (current - 0.05)) && (SUPPLY_PWM < 0xFF))
				{
					SUPPLY_PWM++;
				}
				else if ((batteryCurrent > (current + 0.05) && (SUPPLY_PWM > 0x00)))
				{
					SUPPLY_PWM--;
				}
			}
	
		}

		
		lcd.lcd_clear();
		lcd.lcd_goto_xy(0, 0);
		lcd.lcd_string_format("Vout: %5.2f \nIout: %5.2f \nP: %5.2f W\nPWM: %03d\n", batteryVoltage, batteryCurrent, (batteryVoltage * batteryCurrent), SUPPLY_PWM);

		if (constV)
		{
			lcd.lcd_str("CONST V");
		}
		else
		{
			lcd.lcd_str("CONST C");
		}
	}

	PWM_STOP;
	PWM_RESET;
	TIMER1_STOP;

	while (BTN_MIDDLE_PRESSED);
}