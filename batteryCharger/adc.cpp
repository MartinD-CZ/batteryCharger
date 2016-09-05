/*
 * AVR MultiChemistry charger
 * file containing all ADC functions
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */  

 #include <avr/io.h>
 #include "adc.h"
 #include "header.h"
 #include <util/delay.h>

 void ADC_setup(void)
 {
	 ADCSRA |= (1<<ADEN);	//enables ADC
	 ADCSRA |= (1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);   //selects prescaler
 }

 float measureBatteryVoltage(void)
 {
	 uint16_t samples = 0;
	 ADMUX = (1<<REFS0)|(1<<MUX0);
	 _delay_us(250);

	 for (uint8_t i = 0; i < 8; i ++)
	 {
		 ADCSRA |= (1<<ADSC);
		 while(ADCSRA & (1<<ADSC));
		 samples += ADC;
	 }

	 return((samples * VOLTAGE_GAIN * ADC_INPUT_VOLTAGE)/(8 * 1024));		//Vout = (samples * Vref * 3) / (4 * 1024)		Vref = 5V

 }

 float measureChargingCurrent(void)
 {
	 uint16_t samples = 0;
	 ADMUX = (1<<REFS0);
	 _delay_us(250);

	 for (uint8_t i = 0; i < 8; i ++)
	 {
		 ADCSRA |= (1<<ADSC);
		 while(ADCSRA & (1<<ADSC));
		 samples += ADC;
	 }

	 return((samples * CURRENT_GAIN * ADC_INPUT_VOLTAGE)/(8 * 1024 * SHUNT_RESISTOR_CHARGE));

 }

 float measureDischargingCurrent(void)
 {
	 uint16_t samples = 0;
	 ADMUX = (1<<REFS0)|(1<<MUX1);
	 _delay_us(250);

	 for (uint8_t i = 0; i < 8; i ++)
	 {
		 ADCSRA |= (1<<ADSC);
		 while(ADCSRA & (1<<ADSC));
		 samples += ADC;
	 }

	 return((samples * CURRENT_GAIN * ADC_INPUT_VOLTAGE)/(8 * 1024 * SHUNT_RESISTOR_DISCHARGE));
 }

 uint16_t measureBatteryTemperature(void)		//designed for LM35DZ temperature sensor
 {
	 uint16_t samples = 0;
	 ADMUX = (1<<REFS0)|(1<<MUX1)|(1<<MUX0);
	 _delay_us(250);

	 for (uint8_t i = 0; i < 8; i ++)
	 {
		 ADCSRA |= (1<<ADSC);
		 while(ADCSRA & (1<<ADSC));
		 samples += ADC;
	 }

	 return(samples / (8 * LM35_ADC_PER_DEG));
 }
