#include "display.h"
#include <U8g2lib.h>

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R2, 99, 10, 9);		//we are not using CE pin, so I declared it to be 99 (which of course does not exist)

//Rotary encoder variables, serviced by PCINT2 ISR
volatile int8_t encoder = 0;
volatile bool btn_pressed = 0;

#define CHAR_PER_LINE	10						//how many characters fit per line?
#define FONT_NORMAL		u8g2_font_t0_12_tf
#define FONT_LARGE		u8g2_font_t0_16_tf

void display_init(void)
{
	u8g2.begin();
	u8g2.setContrast(140);
	u8g2.setFont(FONT_NORMAL);
}

void display_msg(const char *title, const char *message)
{
	u8g2.clearBuffer();
	u8g2.setFont(FONT_NORMAL);

	u8g2.drawBox(0, 0, 96, 12);
	u8g2.setDrawColor(0);
	u8g2.drawStr(2, 11, title);
	u8g2.setDrawColor(1);
	
	//uint8_t length = sizeof(message);
	
	u8g2.drawStr(0, 24, message);
	u8g2.sendBuffer();

	btn_pressed = 0;
	while (!btn_pressed);
	_delay_ms(100);
	btn_pressed = 0;
}



uint8_t display_drawMenu(const char* entries[])
{
	u8g2.clearBuffer();

	u8g2.drawBox(0, 0, 96, 12);
	u8g2.setDrawColor(0);
	u8g2.drawStr(2, 11, entries[0]);
	u8g2.setDrawColor(1);
	
	for (uint8_t i = 1; i < 5; i++)
		u8g2.drawStr(5, i * 9 + 12, entries[i]);

	return display_drawMenuLoop();
}

uint8_t display_drawMenuLoop(void)
{
	int8_t cursorPos = 1;
	encoder = 0;
	btn_pressed = false;

	while (!btn_pressed)
	{
		u8g2.setDrawColor(0);
		u8g2.drawBox(0, 13, 5, 36);
		u8g2.setDrawColor(1);
		
		u8g2.drawStr(0, cursorPos * 9 + 12, ">");
		u8g2.sendBuffer();

		encoder = 0;
		while ((encoder == 0) & !btn_pressed);
		_delay_ms(75);
		
		cursorPos += encoder;
		encoder = 0;
		
		if (cursorPos < 1)	
			cursorPos = 4;

		if (cursorPos > 4)
			cursorPos = 1;
	}
	btn_pressed = 0;
	_delay_ms(100);

	return (cursorPos - 1);
}

uint16_t display_drawNumMenu(const char* name, const char* unit, uint16_t defValue, uint16_t minValue, uint16_t maxValue, uint16_t incValue)
{
	u8g2.clearBuffer();

	u8g2.drawBox(0, 0, 96, 12);
	u8g2.setDrawColor(0);
	u8g2.drawStr(2, 11, name);
	u8g2.setDrawColor(1);

	int16_t value = defValue;

	while (!btn_pressed)
	{
		u8g2.setDrawColor(0);
		u8g2.drawBox(0, 13, 48, 96);
		u8g2.setDrawColor(1);

		u8g2.setFont(FONT_LARGE);
		char buffer[10];
		itoa(value, buffer, 10);
		u8g2.drawStr(5, 30, buffer);
		u8g2.drawStr(60, 30, unit);
		u8g2.drawStr(32, 48, "OK");
		u8g2.setFont(FONT_NORMAL);

		u8g2.sendBuffer();

		encoder = 0;
		btn_pressed = 0;
		while ((encoder == 0) & !btn_pressed);
		value += encoder * incValue;
		_delay_ms(150);
		encoder = 0;
	}
	_delay_ms(50);
	encoder = 0;
	btn_pressed = 0;

	return value;
}

void display_drawFunction(const char* name, const char* line1, const char* line2, const char* line3, const char* line4)
{
	u8g2.clearBuffer();

	u8g2.drawBox(0, 0, 96, 12);
	u8g2.setDrawColor(0);
	u8g2.drawStr(2, 11, name);
	u8g2.setDrawColor(1);

	u8g2.drawStr(0, 21, line1);
	u8g2.drawStr(0, 30, line2);
	u8g2.drawStr(0, 39, line3);
	u8g2.drawStr(0, 48, line4);

	u8g2.sendBuffer();
}

uint8_t a0, c0;
int8_t tmp;
//encoder routine taken from http://www.technoblogy.com/show?1YHJ
ISR(PCINT2_vect)
{
	if (!(PIND & (1 << PIND4)))			//read the button on the rotary encoder
		btn_pressed = 1;
	else
	{
		uint8_t a = PIND >> 2 & 1;
		uint8_t b = PIND >> 3 & 1;
		if (a != a0) 
		{             
			a0 = a;
			if (b != c0) 
			{
				c0 = b;
				if (a == b)
					tmp--;
				else
					tmp++;

				if (tmp > 1)
				{
					tmp = 0;
					encoder++;
				}
				else if (tmp < -1)
				{
					tmp = 0;
					encoder--;
				}
					
			}
		}
	}
}



