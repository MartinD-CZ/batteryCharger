/*
 * AVR MultiChemistry charger
 * header file
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */ 


#ifndef CONSTANTCV_H_
#define CONSTANTCV_H_

#define RUNNING				0
#define REGULAR				1
#define OVERTEMPERATURE		2
#define USER				3


void constantCVCharge(float maxCurrent, float maxVoltage);

#endif /* CONSTANTCV_H_ */