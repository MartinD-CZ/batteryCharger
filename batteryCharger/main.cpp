/*
 * AVR MultiChemistry charger
 * main.cpp file
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 

#include <avr/wdt.h>
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));		//disables watchdog on startup

#include <inttypes.h>
#include "uart/uart.h"
#include <avr/io.h>
#include "header.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "display/5110.h"
#include "display/menu.h"
extern LCD_5110 lcd;
#include "adc.h"
#include "batteryTypes/constantCV.h"
#include "batteryTypes/NiMH.h"
#include "functions.h"

//variables
volatile bool secondFlag;

char* mainMenuName = "Select mode: ";
char* mainMenuItems[5] = {"CHARGE", "DISCHARGE", "POWER SUPPLY", "ABOUT", "RESTART"};
char* chemistryMenuName = "Battery type:";
char* chemistryMenuItems[5] = {"Li-Ion/SLA", "NiCd/NiMH", "CONST V", "CONST C", "BACK TO MENU"};

int main(void)
{
    PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4);	//button pull up resistors 
	PORTB |= (1<<PORTB1)|(1<<PORTB2);

	DDRD |= (1<<DDD7);				//LED pins as outputs
	DDRB |= (1<<DDB0);	

	DDRD |= (1<<DDD5)|(1<<DDD6);	//pwm pins as outputs

	DDRC |= (1<<DDC4)|(1<<DDC5);	//UART and fan pins as outputs
	UART_PIN_HIGH;

	//setting up timer 1
	TCNT1 = 0;
	OCR1A = 19531;				//set the overflow value (overflow each second)
	TIMSK1 |= (1<<OCIE1A);		//enable CTC interrupt
	sei();						//globally enable interrupts

	ADC_setup();				
	lcdInit();					

	while (1) 
    {
		lcd.lcd_clear();
		float maxC, maxV, disC, minV;

		switch (drawSelectionMenu(mainMenuName, mainMenuItems))
		{
			case 0:		//charge
				uint8_t cells, hrs;
				switch(drawSelectionMenu(chemistryMenuName, chemistryMenuItems))
				{
					case 0:		//Li-Ion/SLA
						maxC = drawNumericalMenu("max charging\ncurrent [A]", 1.00);
						maxV = drawNumericalMenu("max battery\nvoltage [V]", 4.20);
						constantCVCharge(maxC, maxV);
						break;
					case 1:		//NiCd/NiMH
						maxC = drawNumericalMenu("charging\ncurrent [A]", 1.00);
						cells = drawNumericalMenu("# of\ncells", 1.00);
						hrs = drawNumericalMenu("charging\ntime [Hrs]", 7);
						chargeNiMH(maxC, cells, hrs);
						break;
					case 2:		//constant voltage
						maxV = drawNumericalMenu("output\nvoltage [V]", 5.00);
						constantVoltage(maxV);
						break;
					case 3:		//constant current
						maxC = drawNumericalMenu("output\ncurrent [A]", 1.00);
						constantCurrent(maxC);
						break;
					case 4:		//return
						break;
				}
				break;
			case 1:		//discharge
				disC = drawNumericalMenu("discharging\ncurrent [A]", 1.00);
				minV = drawNumericalMenu("minimal\nvoltage [A]", 1.00);
				discharge(disC, minV);
				break;
			case 2:		//power supply - constant current and voltage limited power supply (WARNING - long 
				maxV = drawNumericalMenu("output\nvoltage [V]", 5.00);
				maxC = drawNumericalMenu("current\nlimit [A]", 1.00);
				constantCV(maxV, maxC);
				break;
			case 3:		//about - just display a screen on the lcd
				lcd.lcd_clear();
				lcd.lcd_goto_xy_exact(0, 0);
				lcd.lcd_str(" AVR Charger   by M. Danek  V1.10  (c)2016   madanek\n   post.cz");
				while (!BTN_MIDDLE_PRESSED);
				while (BTN_MIDDLE_PRESSED);
				break;
			case 4:		//restart (set up a watchdog)
				wdt_enable(WDTO_15MS);
				while (1);
				break;
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	secondFlag = true;
}

void wdt_init(void)
{
	MCUSR = 0;
	wdt_disable();

	return;
}
