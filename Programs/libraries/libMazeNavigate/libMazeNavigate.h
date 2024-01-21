#ifndef LIBMAZENAVIGATE_H
#define LIBMAZENAVIGATE_H
#include <Arduino.h>

int findNextCell(int lTrig, int fTrig, int rTrig, int lEcho, int fEcho, int rEcho);
String findFreeCells(int lTrig, int fTrig, int rTrig, int lEcho, int fEcho, int rEcho);

#endif 