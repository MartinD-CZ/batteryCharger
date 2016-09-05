/*
 * AVR MultiChemistry charger
 * header containing definitions for main.cpp
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 


#ifndef HEADER_H_
#define HEADER_H_

#define F_CPU					20000000
#define UART					true

//fan
#define FAN_ON					PORTC |= (1<<PORTC5)
#define FAN_OFF					PORTC &= ~(1<<PORTC5)

//LEDs
#define RED_LED_ON				PORTD |= (1<<PORTD7)
#define YEL_LED_ON				PORTB |= (1<<PORTB0)

#define RED_LED_OFF				PORTD &= ~(1<<PORTD7)
#define YEL_LED_OFF				PORTB &= ~(1<<PORTB0)

#define RED_LED_TOGGLE			PORTD ^= (1 << PORTD7)
#define YEL_LED_TOGGLE			PORTB ^= (1 << PORTB0)

//buttons
#define BTN_LEFT_PRESSED		!(PINB & (1<<PINB2))
#define BTN_RIGHT_PRESSED		!(PINB & (1<<PINB1))
#define BTN_MIDDLE_PRESSED		!(PIND & (1<<PIND4))
#define BTN_DOWN_PRESSED		!(PIND & (1<<PIND3))
#define BTN_UP_PRESSED			!(PIND & (1<<PIND2))

//ADC definitions
#define SHUNT_RESISTOR_CHARGE	0.1
#define SHUNT_RESISTOR_DISCHARGE 0.1
#define CURRENT_GAIN			0.0625
#define VOLTAGE_GAIN			3.035
#define ADC_INPUT_VOLTAGE		5.00
#define LM35_LINEARITY			0.01											//aka 0.01 V per °C
#define LM35_ADC_PER_DEG		((LM35_LINEARITY*1024)/ADC_INPUT_VOLTAGE)		//how many ADC digital values equal one degree

//PWM definitions
#define SUPPLY_PWM				OCR0B
#define LOAD_PWM				OCR0A
#define PWM_START				TCCR0B |= (1<<CS00)
#define PWM_STOP				TCCR0B &= (~CS00)
#define PWM_SUPPLY_MODE			TCCR0A=(1<<WGM01)|(1<<WGM00)|(1<<COM0B1)
#define PWM_LOAD_MODE			TCCR0A=(1<<WGM01)|(1<<WGM00)|(1<<COM0A1)
#define PWM_RESET				TCCR0A = 0

#define TIMER1_START			TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS12)     
#define TIMER1_STOP				TCCR1B = 0

#define MAX_TEMPERATURE			45


#endif /* HEADER_H_ */