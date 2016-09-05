/*
 * AVR MultiChemistry charger
 * header for UART transmitter library
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */  


#ifndef UART_H_
#define UART_H_





#define UART_PIN_HIGH		PORTC |= (1<<PORTC4)
#define UART_PIN_LOW		PORTC &=~(1<<PORTC4)
#define UART_BAUDRATE_DELAY	16		//57600
//#define UART_BAUDRATE_DELAY 25	//38400

void uart_sendChar(char c);
void uart_sendStr(char *str);
void uart_sendStrFormatted(char *szFormat, ...);


#endif /* UART_H_ */