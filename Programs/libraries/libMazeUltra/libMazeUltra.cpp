#include "libMazeUltra.h"
#include <Arduino.h>

float ultra(int trigPin, int echoPin){
    long duration;
    int distance;

    // Clears the trigPin.
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Sets the trigPin on HIGH state for 10 micro seconds.
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds.
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance in cm.
    distance = duration * 0.034 / 2; 
    delayMicroseconds(50);
    return distance;
}