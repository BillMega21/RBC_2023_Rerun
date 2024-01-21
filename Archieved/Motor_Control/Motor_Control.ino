  // Pins for controlling the direction and speed of motor A
  int ena = 0;
  int in1 = 1;
  int in2 = 2;

  // Pins for controlling the direction and speed of motor B
  int enb = 5;
  int in3 = 4;
  int in4 = 3;

  int d = 2000;
  int wait = 2000;

void setup() {
  //SETTING UP CONNECTION MODES
  pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);

  pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);

  //setting the initial direction of both motors as forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  //setting the initial state of both motors as off (SPEED = 0)
  analogWrite(ena, 0);
  analogWrite(enb, 0);



}

void loop() {
  // put your main code here, to run repeatedly:
  
 delay(wait);
  delay(wait);
  MoveForward(in1,in2,ena,in3,in4,enb,250);
  delay(d);
  EmergencyStop(ena,enb);
  delay(wait);
  
  MoveBackward(in1,in2,ena,in3,in4,enb,250);
  delay(d);
  EmergencyStop(ena,enb);
  delay(wait);
  
  TurnRight(in1,in2,ena,in3,in4,enb,250);
  delay(d);
  EmergencyStop(ena,enb);
  delay(wait);
  
  TurnLeft(in1,in2,ena,in3,in4,enb,250);
  delay(d);
  EmergencyStop(ena,enb);
  delay(wait);




}

void MoveForward(int in1,int in2,int ena,int in3,int in4,int enb,int speed)
{
  //setting the direction of both motors as forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  //setting the speed of both motors
  analogWrite(ena, speed);//  speed has to be between 0 and 255
  analogWrite(enb, speed);
}

void MoveBackward(int in1,int in2,int ena,int in3,int in4,int enb,int speed)
{
  //setting the direction of both motors as backward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  //setting the speed of both motors
  analogWrite(ena, speed);//  speed has to be between 0 and 255
  analogWrite(enb, speed);
}

void TurnRight(int in1,int in2,int ena,int in3,int in4,int enb,int speed)
{
  //setting the direction of motor a as backward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  //setting the direction of motor b as forward
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(ena, speed);
  analogWrite(enb, speed);
}
void TurnLeft(int in1,int in2,int ena,int in3,int in4,int enb,int speed)
{
  //setting the direction of motor a as forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  //setting the direction of motor b as backward
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(ena, speed);
  analogWrite(enb, speed);

}

void EmergencyStop(int ena,int enb)//might wanna consider globally having the enable pins rather than having them as inputs
{
    analogWrite(ena, 0);
    analogWrite(enb, 0);
}