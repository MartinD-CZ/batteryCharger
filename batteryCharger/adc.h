/*
 * AVR MultiChemistry charger
 * header for adc.h
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC_setup(void);
float measureBatteryVoltage(void);
float measureChargingCurrent(void);
float measureDischargingCurrent(void);
uint16_t measureBatteryTemperature(void);

#endif /* ADC_H_ */