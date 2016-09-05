/*
 * AVR MultiChemistry charger
 * file with graphic menu prototypes
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */

#include "..\header.h"
#include <avr/io.h>
#include <util/delay.h>
#include "5110.h"
LCD_5110 lcd;
#include "menu.h"


 void lcdInit(void)
 {
	 lcd.lcd_init(&PORTD, PD0, &PORTD, PD1, &PORTB, PB3, &PORTB, PB4, &PORTB, PB5);
	 
	 lcd.lcd_goto_xy_exact(0, 0);
	 lcd.lcd_string_format(" AVR Charger   by M. Danek  V1.10  (c)2016   madanek\n   post.cz");
	 _delay_ms(3000);
 }

 void updateSelectionMenu(uint8_t pos, char* name, char* items[5])
 {
	 lcd.lcd_clear();

	 lcd.lcd_goto_xy(0, 0);
	 lcd.lcd_str(name);

	 for (uint8_t i = 0; i < 5; i ++)
	 {
		lcd.lcd_goto_xy(1, (i + 1));
		lcd.lcd_str(items[i]);
	 }


	 lcd.lcd_goto_xy(0, pos + 1);
	 lcd.lcd_str(">");

 }
 uint8_t drawSelectionMenu(char* name, char* items[5])
 {
	 uint8_t menuPos = 0;
	 
	 updateSelectionMenu(menuPos, name, items);

	 while (!BTN_MIDDLE_PRESSED)
	 {
		 if (BTN_DOWN_PRESSED && (menuPos < 4))
		 {
			 menuPos++;
			 while (BTN_DOWN_PRESSED);
			 updateSelectionMenu(menuPos, name, items);
		 }

		 if (BTN_UP_PRESSED && (menuPos > 0))
		 {
			 menuPos--;
			 while (BTN_UP_PRESSED);
			 updateSelectionMenu(menuPos, name, items);
		 }

		 _delay_ms(100);
	 }
	 while (BTN_MIDDLE_PRESSED);

	 return menuPos;
 }

 void updateNumericalMenu(char* menuName, float value, uint8_t cursorPos)
 {
	 lcd.lcd_clear();

	 lcd.lcd_goto_xy(0, 0);
	 lcd.lcd_str(menuName);
	 
	 lcd.lcd_goto_xy(0, 2);
	 lcd.lcd_string_format("%05.2f", value);

	 lcd.lcd_goto_xy(cursorPos, 3);
	 lcd.lcd_str("^");
 }

 float drawNumericalMenu(char* menuName, float defaultValue)
 {
	 float value = defaultValue;
	 uint8_t menuPos = 0;

	 updateNumericalMenu(menuName, value, menuPos);

	 while (!BTN_MIDDLE_PRESSED)
	 {
		 if (BTN_RIGHT_PRESSED && (menuPos < 4))
		 {
			 menuPos++;
			 while (BTN_RIGHT_PRESSED);
			 updateNumericalMenu(menuName, value, menuPos);
		 }

		 if (BTN_LEFT_PRESSED && (menuPos > 0))
		 {
			 menuPos--;
			 while (BTN_LEFT_PRESSED);
			 updateNumericalMenu(menuName, value, menuPos);
		 }

		 if (BTN_UP_PRESSED)
		 {
			 switch (menuPos)
			 {
				 case 0:
				 value += 10.0;
				 break;
				 case 1:
				 value += 1.0;
				 break;
				 case 2:
				 break;
				 case 3:
				 value += 0.1;
				 break;
				 case 4:
				 value += 0.01;
				 break;
			 }
			 while (BTN_UP_PRESSED);
			 updateNumericalMenu(menuName, value, menuPos);
		 }

		 if (BTN_DOWN_PRESSED)
		 {
			 switch (menuPos)
			 {
				 case 0:
				 value -= 10.0;
				 break;
				 case 1:
				 value -= 1.0;
				 break;
				 case 2:
				 break;
				 case 3:
				 value -= 0.1;
				 break;
				 case 4:
				 value -= 0.01;
				 break;
			 }
			 while (BTN_DOWN_PRESSED);
			 updateNumericalMenu(menuName, value, menuPos);
		 }
		 
		 _delay_ms(100);
	 }
	 while (BTN_MIDDLE_PRESSED);

	 return value;
 }