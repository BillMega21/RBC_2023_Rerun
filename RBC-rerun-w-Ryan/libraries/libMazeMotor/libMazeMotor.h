#ifndef LIBMAZEMOTOR_H
#define LIBMAZEMOTOR_H
#include <Arduino.h>

void stop(int delay_time, int in1, int in2, int in3, int in4, int ena, int enb);
void moveBackward(int in1, int in2, int in3, int in4, int ena, int enb, int speed);
int validateDir(int dir);
void rotateRight(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb);
void rotateLeft(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb);
void rotate180(int speed, int turn_90_time, int in1, int in2, int in3, int in4, int ena, int enb);

#endif 