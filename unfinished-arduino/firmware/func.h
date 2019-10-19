#ifndef FUNC_H
#define FUNC_H

void func_chargeLi(uint16_t tgtVolt, uint16_t tgtAmp, uint16_t cutOffI);
void func_printf(uint8_t bufIndex, char *szFormat, ...);
void func_discharge(uint16_t current, uint16_t minVolt);

#endif
