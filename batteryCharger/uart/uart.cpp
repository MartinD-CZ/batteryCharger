/*
 * AVR MultiChemistry charger
 * very simple software UART transmitter library
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 

#define F_CPU 20000000
 
#include <avr/io.h>
#include "uart.h"
#include <stdio.h>
#include <util/delay.h>

//function for sending chars (the core of the whole library)
void uart_sendChar(char c)
{
	UART_PIN_LOW;
	 _delay_us(UART_BAUDRATE_DELAY);		//sending at baud 57600
	 
	 for (int i = 0; i < 8; ++i)
	 {

		 if ((c >> i) & 1)
		 {
			 UART_PIN_HIGH;
		 }
		 else
		 {
			 UART_PIN_LOW;
		 }
		 _delay_us(UART_BAUDRATE_DELAY);
	 }

	 UART_PIN_HIGH;
	 _delay_us(UART_BAUDRATE_DELAY);
 }

//function for sending strings
void uart_sendStr(char *str)
{
	while(*str)  uart_sendChar(*str++);
}

//function for sending string formatted using printf syntax (if you want to send floats, you must enable vprintf library!)
//How to enable vprintf in Atmel Studio 7: In project properties, toolchain, under AVR/GNU Linker, general, check "Use vprintf library(-WI,-u,vfprintf)" AND under AVR/GNU Linker, Miscelllaneous, in other linker flags enter: -Wl,-lprintf_flt
 void uart_sendStrFormatted(char *szFormat, ...)
 {
	char szBuffer[64]; 
	int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	
	uart_sendStr(szBuffer);
 }