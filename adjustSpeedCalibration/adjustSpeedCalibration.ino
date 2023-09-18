#define in3 2        // LEFT MOTOR
#define in4 4        // LEFT MOTOR
#define enb 9        // LEFT MOTOR

#define in1 7        // RIGHT MOTOR
#define in2 8        // RIGHT MOTOR
#define ena 6        // RIGHT MOTOR

#define led 19

#define s0_color 14       // COLOUR SENSOR
#define s1_color 15       // COLOUR SENSOR
#define s2_color 16       // COLOUR SENSOR
#define s3_color 17       // COLOUR SENSOR
#define out_color 18      // COLOUR SENSOR

#define fTrig 13                                 // ULTRASONIC
#define fEcho 12                                 // ULTRASONIC
#define lTrig 11                                 // ULTRASONIC
#define lEcho 5                                  // ULTRASONIC
#define rTrig 10                                 // ULTRASONIC
#define rEcho 3                                  // ULTRASONIC
const int TrigPins[3] = {lTrig, fTrig, rTrig};   // ULTRASONIC
const int EchoPins[3] = {lEcho, fEcho, rEcho};   // ULTRASONIC


#define BAUDRATE 9600

void setup() {
  // Arduino Pin Initialisation
  Serial.begin(BAUDRATE);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);

  pinMode(rTrig, OUTPUT);
  pinMode(rEcho, INPUT);
  pinMode(lTrig, OUTPUT);
  pinMode(lEcho, INPUT);
  pinMode(fTrig, OUTPUT);
  pinMode(fEcho, INPUT);

  pinMode(led, OUTPUT);
  stop();
  while (Serial.read() != '\n') {} // wait
}

int speed_normal = 100;
int speed_right = speed_normal;
int speed_left = speed_normal;
int speed_offset = 15;

int adjust_rate = 200;
int scaling_factor = 2;
int scaling_factor_far = 4;

int dif_threshold = 7;

// for ultra to stop
int front_threshold = 4;

//int counter = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = 0;

void loop() { 

  //setting the direction of both motors as forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  adjustSpeed(); //love u

  //setting the speed of both motors
  analogWrite(enb, speed_right);
  analogWrite(ena, speed_left);



  //    int front_dist = ultra(fTrig, fEcho);
  //    Serial.print("Front distance: ");
  //    Serial.println(front_dist);
  //    if ( front_dist < front_threshold){
  //        counter += 1;
  //    }
  //    if (counter > 2){
  //        stop();
  //        digitalWrite(led, HIGH);
  //        Serial.println("STOPPPPPPPPPPPPP\n");
  //        Serial.println(front_dist);
  //        Serial.println("\n\n\n\n");
  //        delay(10000000);
  //    }
}


// ##########################################################
// ##########################################################
// ########     Helper function #############################
// ##########################################################
// ##########################################################

void adjustSpeed() {
  int dif_dist;
  int adjust_val, left_adjust, right_adjust;
  int s;

  // Get the distance reading from left and riht ultra
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // note: use integer first! - might need to change for more accuracy
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  float left_dist = ultra(lTrig, lEcho);
  float right_dist = ultra(rTrig, rEcho);
  Serial.print("Left: ");     Serial.print(left_dist);
  Serial.print("; Right: ");  Serial.println(right_dist);
  
  // NOTEEEEEEEEE
  // Only change the speed for delay_rate:
  currentMillis = millis();
  if (currentMillis - previousMillis >= adjust_rate) {
    Serial.println(">> Update speed <<");
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  
    // Compute the adjusting distance
    dif_dist = abs(left_dist - right_dist);
  
    // Compute the adjust value
    if (dif_dist > dif_threshold ) {
      s = scaling_factor_far;
      Serial.print(" -- NEAR WALL -- ");
    } else {
      s = scaling_factor;
      Serial.print(" -- ~centered -- ");
    }
  
  
    adjust_val = s * dif_dist;
  
    if (left_dist > right_dist) {       // need to steer left --> decrease left
      left_adjust     = - adjust_val;
      right_adjust    = adjust_val;
    } else {                            // need to steer right --> decrease right
      left_adjust     = adjust_val;
      right_adjust    = - adjust_val;
    }
  
    // Update the speed
    speed_left = speed_normal + left_adjust;
    speed_right = speed_normal + right_adjust;
  
    // Apply offset
    speed_left = speed_left - speed_offset;
  
    Serial.print("\t-->");
    Serial.print(speed_left);
    Serial.print("\t");
    Serial.println(speed_right);
  
    // TUNIGGGGGGGGGGGGGGGG :(
  }
}

float ultra(int trigPin, int echoPin) {
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

// ################################################################
void stop() {
  //setting the direction of both motors as forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(ena, 0);
  analogWrite(enb, 0);
}
