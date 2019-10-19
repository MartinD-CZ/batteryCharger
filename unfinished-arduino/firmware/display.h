// display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


void display_init(void);
void display_msg(const char *title, const char *message);
//void display_printf(char *szFormat, ...);
uint8_t display_drawMenu(const char* entries[]);
uint8_t display_drawMenuLoop(void);
uint16_t display_drawNumMenu(const char* name, const char* unit, uint16_t defValue, uint16_t minValue, uint16_t maxValue, uint16_t incValue);
void display_drawFunction(const char* name, const char* line1, const char* line2, const char* line3, const char* line4);



#endif

