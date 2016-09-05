/*
 * AVR MultiChemistry charger
 * header
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void discharge(float maxCurrent, float minVoltage);
void constantVoltage(float voltage);
void constantCurrent(float current);
void constantCV(float voltage, float current);


#endif /* FUNCTIONS_H_ */