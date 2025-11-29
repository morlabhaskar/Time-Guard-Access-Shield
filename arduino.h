#ifndef __ARDUINO_H__
#define __ARDUINO_H__
#include "types.h"

void pinMode(u32 pinNo,u32 pinDir);
void digitalWrite(u32 pinNo,u32 bit);
u32 digitalRead(u32 pinNo);

void portMode(u32 pinStartNo,u32 nPins,u32 pinsDir);
void write2Pins(u32 pinStartNo,u32 nPins,u32 data);
u32 readPins(u32 pinStartNo,u32 nPins);

#endif
