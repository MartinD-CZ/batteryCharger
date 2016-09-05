/*
 * AVR MultiChemistry charger
 * header
 * (C) Martin Danek, 2016
 * madanek@post.cz
 * github.com/MartinD-CZ/AVRcharger
 */


#ifndef DRAW_H_
#define DRAW_H_

#include <inttypes.h>

void lcdInit(void);
void updateSelectionMenu(uint8_t pos, char* name, char* items[5]);
uint8_t drawSelectionMenu(char* name, char* items[5]);
void updateNumericalMenu(char* menuName, float value, uint8_t cursorPos);
float drawNumericalMenu(char* menuName, float defaultValue);


#endif /* DRAW_H_ */