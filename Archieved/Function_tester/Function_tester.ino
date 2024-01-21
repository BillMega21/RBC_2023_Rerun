#include "my_library.h"
#include "sensor.h"

void setup() 
{
    Serial.begin(9600);
    int result = addTwoInts(4,3);
    printUltra();
    Serial.println(result);
}

void loop() {}
