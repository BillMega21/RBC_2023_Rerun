#include <libMazePrint.h>
#include <libMazeUltra.h>
#include <libMazeMotor.h>
#include <libMazeColour.h>
#include <libMazeNavigate.h>

#define MAZE_SIZE 4
#define BAUDRATE 9600
#define in3 2        // LEFT MOTOR
#define in4 4        // LEFT MOTOR
#define enb 9        // LEFT MOTOR
#define in1 7        // RIGHT MOTOR
#define in2 8        // RIGHT MOTOR
#define ena 6        // RIGHT MOTOR

// --------------------------------------------------------------------------
// MOTOR CONTROL CONSTANTS
// --------------------------------------------------------------------------
const int speed = 150;

int turn_time = 1000; // previous value from COMP

//////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(BAUDRATE);
  // put your setup code here, to run once:
  motorPinSetup();
  
  while (Serial.read() != 's') {} // wait
}

void loop() {
  //   put your main code here, to run repeatedly:
  rotateLeft(speed, turn_time, in1, in2, in3, in4, ena, enb);
  stop(in1, in2, in3, in4, ena, enb);
  rotateRight(speed, turn_time, in1, in2, in3, in4, ena, enb);
  stop(in1, in2, in3, in4, ena, enb);

  //  moveForwardSimple();

}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void moveForwardSimple() {
  //setting the direction of both motors as forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  //setting the speed of both motors
  analogWrite(enb, speed);
  analogWrite(ena, speed);
}


////////////////////////////////////////////////////////////////////////////////////////
///////////////         Set up - init       ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void motorPinSetup() {
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT); pinMode(in1, OUTPUT); pinMode(in2, OUTPUT); pinMode(ena, OUTPUT); pinMode(enb, OUTPUT);
}
