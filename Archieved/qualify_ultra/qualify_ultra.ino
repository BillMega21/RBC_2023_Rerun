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
#define lTrig 11
#define lEcho 10
#define fTrig 8
#define fEcho 9

// Bluetooth
#define RX 6
#define TX 7

#define LED_pin 2

const int speed = 150;
const int speedR = speed;
const int speedL = speed - 20; // min 130 - not enough torque

const int TrigPins[3] = {lTrig,fTrig,rTrig};
const int EchoPins[3] = {lEcho,fEcho,rEcho};
const float ThresholdDist = 15;

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
    Serial.begin(9600);
}

void loop() {
    // moveForward();
    digitalWrite(inA_1, HIGH);
    digitalWrite(inA_2, LOW);
    //setting the direction of motor b as backward
    digitalWrite(inB_1, HIGH);
    digitalWrite(inB_2, LOW);

    feedback();
    
    // blinkGoal();
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

void adjustDir(int enR,int enL,int speedR,int speedL)
{
  analogWrite(enR, speedR);
  analogWrite(enL, speedL);
  Serial.print(speedR);
  Serial.print("  ");
  Serial.println(speedL);
//   delay(500); < fuck u
    delay(10);
}

void feedback()
{
  float distances = ultra(rTrig, rEcho);

    // steer to the right --> left faster
  if (distances > ThresholdDist)
  {
    int k = 2;
    float initSpeed = 150;
    float speedR = (distances - ThresholdDist)*k;
    
    adjustDir(enA, enB, initSpeed+speedR, speedR-initSpeed);
  }
}

void printUltra(float distance[], int trig[], int echo[], int polling_rate){
    // Set up variables
    int sensor_count = sizeof(distance) / sizeof(distance[0]);
    // Get inputs
    for (int i = 0; i < sensor_count; i++) {
        distance[i] = ultra(trig[i], echo[i]);
        delay(20);
    }
    // Print:
    Serial.print("The distance:\t");
    for (int i = 0; i < sensor_count; i++){
        Serial.print(distance[i]);
        Serial.print("\t\t");
    }
    Serial.print("\n");
    // Wait between each measurement
    delay(polling_rate);
}

float ultra(int trigPin, int echoPin) {
    long duration;
    float distance;

    // Clears the trigPin.
    digitalWrite(trigPin, LOW);//////////////I AM NOT SURE IF CAN USE 0 INSTEAD OF LOW
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds.
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds.
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance.
    distance = duration * 0.034 / 2; //output as cm
    return distance;
}

// float* all_ultra(int trigPins[3],int echoPins[3]) //This probably should be within the the ultrasonic file, but I am not sure 
// {
//     //go through all sensors from left right
//     float *distances[3];
//     for (int i; i<3;i++)
//     {
//       distances[i] = ultra(trigPins[i],echoPins[i]);
//     }
//     return *distances;
// }

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