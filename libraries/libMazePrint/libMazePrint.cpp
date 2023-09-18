#include "libMazePrint.h"

void printCoordinates(int x, int y, int dir) {
    Serial.print("x:");
    Serial.print(x);
    Serial.print(", y:");
    Serial.print(y);
    Serial.print(", Current Direction:");
    Serial.println(dir);
}

void printUltra(int distance, int trig, int echo){
    if (trig == 11 and echo == 5){
        Serial.print("Left:");
        Serial.println(distance); 
    }
    else if (trig == 10 and echo == 3){
        Serial.print("Right:");
        Serial.println(distance);
    }
    else if (trig == 13 and echo == 12){
        Serial.print("Front:");
        Serial.println(distance);
    }
}