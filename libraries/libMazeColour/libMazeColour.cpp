#include "libMazeColour.h"
#include <Arduino.h>

int determineColor(int printing_or_not, int s2_color, int s3_color, int out_color, int cal_min_r, int cal_max_r, int cal_min_g, int cal_max_g, int cal_min_b, int cal_max_b, int black_threshold, int white_threshold) {
    // Getting the value from the colour sensor.
    digitalWrite(s2_color, LOW);
    digitalWrite(s3_color, LOW);
    int red = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s3_color, HIGH);
    int blue = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s2_color, HIGH);
    int green = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    int returnvalue;

    // Mapping the colour sensor values.
    red = map(red, cal_min_r, cal_max_r, 255, 0);
    green = map(green, cal_min_g, cal_max_g, 255, 0);
    blue = map(blue, cal_min_b, cal_max_b, 255, 0);

    // Printing the colour value detected.
    if (printing_or_not) {
        Serial.print("Red: "); 
	Serial.print(red);
        Serial.print(", Green: "); 
	Serial.print(green);
        Serial.print(", Blue: "); 
	Serial.println(blue);
    }
    
    // Determine what colour is detected.
    if (red < black_threshold && blue < black_threshold && green < black_threshold ){
	if (printing_or_not){
            Serial.println("New Cell Colour: Black");
	}
        return 0;
    } 
    else if (red > white_threshold && blue > white_threshold && green > white_threshold){
	if (printing_or_not){
            Serial.println("New Cell Colour: White");
	}
        return 1;
    } 
    else{
	if (printing_or_not){
            Serial.println("New Cell Colour: Red");
	}
        return 2;
    }
}
