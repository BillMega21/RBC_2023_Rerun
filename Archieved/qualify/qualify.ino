// #define s0_color
// #define s0_color 14
// #define s1_color 15
// #define s2_color 16
// #define s3_color 17
// #define out_color 18
#define inA_1 4
#define inA_2 3
#define enA 5
#define inB_1 14
#define inB_2 15
#define enB 16

// Ultrasonic Sensor
#define rTrig 13
#define rEcho 12
#define lTrig 10
#define lEcho 11
#define fTrig 8
#define fEcho 9

// Bluetooth
#define RX 6
#define TX 7

#define LED_pin 2

const int speed = 150;
const int speedR = speed + 30;
const int speedL = speed ; 
// min 130 - not enough torque

void setup() {
    pinMode(inA_1, OUTPUT);
    pinMode(inA_2, OUTPUT);
    pinMode(enA, OUTPUT);
    pinMode(inB_1, OUTPUT);
    pinMode(inB_2, OUTPUT);
    pinMode(enB, OUTPUT);

    pinMode(rTrig, OUTPUT);
    pinMode(rEcho, INPUT);
    pinMode(lTrig, OUTPUT);
    pinMode(lEcho, INPUT);
    pinMode(fTrig, OUTPUT);
    pinMode(fEcho, INPUT);

        // pinMode(s0_color, OUTPUT);
        // pinMode(s1_color, OUTPUT);
        // pinMode(s2_color, OUTPUT);
        // pinMode(s3_color, OUTPUT);
        // pinMode(out_color, INPUT);
        // digitalWrite(s0_color, HIGH);
        // digitalWrite(s1_color, HIGH);

    pinMode(RX, INPUT);
    pinMode(TX, OUTPUT);

    pinMode(LED_pin, OUTPUT);
}

void loop() {
    moveForward();
    // delay(2000);
    // rotateLeft();
    // delay(2000);
    // rotateRight();
    // delay(2000);
    // moveBackward();
    // delay(2000);
    // stop();
    // delay(2000);
    // blinkGoal();

    
    // // Journey to the destination.
    // moveForward();  // For 3 blocks.
    // delay(3000);
    // moveLeft();
    // moveForward();  // For 3 blocks.
    // delay(3000);
    // moveLeft();
    // moveForward();  // For 2 blocks.
    // delay(2000);
    // moveLeft();
    // moveForward();  // For 2 blocks.
    // delay(2000)
    // moveRight;
    // moveForward();  // For 1 block.
    // delay(1000);

    // Color sensing

    // Blinking
    blinkGoal();
}

void moveForward() {
    //setting the direction of both motors as forward
    //setting the direction of motor a as forward
    digitalWrite(inA_1, HIGH);
    digitalWrite(inA_2, LOW);
    //setting the direction of motor b as backward
    digitalWrite(inB_1, HIGH);
    digitalWrite(inB_2, LOW);
    //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
    analogWrite(enA, speedR);
    analogWrite(enB, speedL);
}




void blinkGoal() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_pin, HIGH);
    delay(1000);
    digitalWrite(LED_pin, LOW);
    delay(1000);
  }
}



void rotateLeft() {

  //setting the direction of motor a as forward
  digitalWrite(inA_1, LOW);
  digitalWrite(inA_2, HIGH);
  //setting the direction of motor b as backward
  digitalWrite(inB_1, HIGH);
  digitalWrite(inB_2, LOW);
  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(enA, speedR);
  analogWrite(enB, speedL);
}

void rotateRight() {
  //setting the direction of motor a as forward
  digitalWrite(inA_1, HIGH);
  digitalWrite(inA_2, LOW);
  //setting the direction of motor b as backward
  digitalWrite(inB_1, LOW);
  digitalWrite(inB_2, HIGH);
  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(enA, speedR);
  analogWrite(enB, speedL);
}


void moveBackward() {
  //setting the direction of both motors as backward
  //setting the direction of motor a as forward
  digitalWrite(inA_1, HIGH);
  digitalWrite(inA_2, LOW);
  //setting the direction of motor b as backward
  digitalWrite(inB_1, HIGH);
  digitalWrite(inB_2, LOW);
  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(enA, speedR);
  analogWrite(enB, speedL);
}

void stop() {
  //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}