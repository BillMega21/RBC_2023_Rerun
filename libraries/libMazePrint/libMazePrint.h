#ifndef LIBMAZEPRINT_H
#define LIBMAZEPRINT_H
#include <Arduino.h>

void printCoordinates(int x, int y, int dir);
void printUltra(int distance, int trig, int echo);

#endif 