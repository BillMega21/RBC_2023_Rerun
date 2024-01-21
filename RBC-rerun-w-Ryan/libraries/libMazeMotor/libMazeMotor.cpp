#include "libMazeMotor.h"
#include <Arduino.h>

void stop(int delay_time, int in1, int in2, int in3, int in4, int ena, int enb){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

    analogWrite(ena, 0);
    analogWrite(enb, 0);

    delay(2000);
}

void moveBackward(int speed, int in1, int in2, int in3, int in4, int ena, int enb)
{
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    analogWrite(ena, speed);
    analogWrite(enb, speed);
}

// To cap the direction to 0-360
int validateDir(int dir){
    if (dir > 360){
        return dir - 360;
    }
    else if (dir < 0){
        return dir + 360;
    }
    else{
        return dir;
    }
}

void rotateLeft(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb)
{

    // setting the direction of motor a as forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    // setting the direction of motor b as backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    // setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
    analogWrite(ena, speed);
    analogWrite(enb, speed);

    delay(700); // Stops after 100ms MAYBE NOT NEEDED
    stop(in1, in2, in3, in4, ena, enb);
}

void rotateRight(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb)
{
    // setting the direction of motor a as forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    // setting the direction of motor b as backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    // setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
    analogWrite(ena, speed);
    analogWrite(enb, speed);
    delay(700); // Stops after 100ms MAYBE NOT NEEDED
    stop(in1, in2, in3, in4, ena, enb);
}

void rotate180(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb)
{
    rotateRight(speed, turn_90_time);
    rotateRight(speed, turn_90_time);
}
