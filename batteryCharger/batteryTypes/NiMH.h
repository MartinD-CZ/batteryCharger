/*
 * AVR MultiChemistry charger
 * header
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */


#ifndef NIMH_H_
#define NIMH_H_

#define NEG_dV				0.20		
#define MAX_CELL_VOLTAGE	1.78

#define RUNNING				0
#define OVERVOLTAGE			1
#define OVERTEMPERATURE		2
#define USER				3
#define deltaV				4
#define TIME				5

void chargeNiMH(float maxCurrent, uint8_t numberOfCells, uint8_t maxHrs);
		 
#endif /* NIMH_H_ */