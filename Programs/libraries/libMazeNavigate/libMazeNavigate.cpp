#include "libMazeNavigate.h"
#include <libMazeUltra.h>
#include <Arduino.h>

int findNextCell(int lTrig, int fTrig, int rTrig, int lEcho, int fEcho, int rEcho){
    // Planning index format (0 means free cell left, 1 means free cell front, 2 means free cell right)
    // Need to consider what is there are multiple free cell????

    // Holds the cell detail of which cell is accessible from current position.
    String inputString = findFreeCells(lTrig, fTrig, rTrig, lEcho, fEcho, rEcho); 
    int inputLength = inputString.length(); 
    int index = 0;
    for (int i = 0; i < inputLength; i++) {
        int intValue = inputString.charAt(i) - '0'; // Convert char to int
        if (intValue == 1) {
            return index;
        }
        index++;
    }
    return index;

    
}

String findFreeCells(int lTrig, int fTrig, int rTrig, int lEcho, int fEcho, int rEcho){
    // A function supposed to get the readings from the three ultrasonic sensors and compare it with a threshold value 
    // to determine whether there is a wall or free space in each direction.

    // This function outputs a 1x3 array, with values of 0 and 1 for each element, 1 for available (no Wall) and 0 for 
    // unavailable (Wall), the three values correspond to [left,front,right].

    float threshold = 20.0;
    int TrigPins[3] = {lTrig,fTrig,rTrig};
    int EchoPins[3] = {lEcho,fEcho,rEcho};
    float distances[3];

    for (int i = 0; i < 3; i++){
        distances[i] = ultra(TrigPins[i], EchoPins[i]);
    }

    int availability[3];
    for (int i = 0; i < 3; i++) {
        if (distances[i] > threshold) {
            availability[i] = 1;
        } else {
            availability[i] = 0;
        }
    }

    String result = "";
    for (int i = 0; i < 3; i++) {
        result += String(availability[i]);
    }
    return result;
}
