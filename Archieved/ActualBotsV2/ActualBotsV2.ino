// Challenge Constraints
#define MAZE_SIZE 4
#define BAUDRATE 9600
// Pin declare ___________________________________
#define RX 3
#define TX 2
#define LED_PIN 14
// MOTOR ___________________________________
// left
#define in3 2
#define in4 4
#define enb 9 //5 (What is the point of this enable pin????)
// right
#define in1 7
#define in2 8
#define ena 6  //3 (What is the point of this enable pin????)
// color ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define s0_color 14
#define s1_color 15
#define s2_color 16
#define s3_color 17
#define out_color 18
// Calibrated constant
// WHITE ------------
#define cal_min_r 5
#define cal_min_g 5
#define cal_min_b 5
// BLACK ------------
#define cal_max_r 46
#define cal_max_g 51
#define cal_max_b 41
#define black_threshold 40
#define white_threshold 200
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ultrasonic
#define fTrig 13
#define fEcho 12
#define lTrig 11
#define lEcho 5
#define rTrig 10
#define rEcho 3
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Motor Control
int speed_normal = 100;
int speed_right = speed_normal;
int speed_left = speed_normal;
int adjust_rate = 200;
int scaling_factor = 2.5;
int scaling_factor_far = 1;
int dif_threshold = 7;





// Constant Definitions
const int START_X = 3;
const int START_Y = 3;
const int START_DIR = 90;
const int START_COLOR = 2;
const int TrigPins[3] = { lTrig, fTrig, rTrig };
const int EchoPins[3] = { lEcho, fEcho, rEcho };

// Motor Speed
const int speed = 180;

// Colour Values
const int all_COLORS[] = { 0, 1, 2, 3, 4, 5, 6 }; // [B, W, R, b, g, p, y]
const int SPECIAL_COLORS[] = { 3, 4, 5, 6 };
const int GATE_DELAY = 3000;

// Global Variable
int current_dir;
int current_xcoor;
int current_ycoor;
char current_color;
int current_dist;

// Bluetooth Definitions
// int new_line = 1;  // For bluetooth

/* maze[]:
    * init with all -1 meaning that it unexplored yet
    * use to store "distance" values of each CELL (that we have gone through, corresponding to start cell) */
int maze[MAZE_SIZE][MAZE_SIZE];


/* shortcut_loca[]:
    * init with all 0
    * use to store special_COLOR_integer_value */
int shortcut_loca[MAZE_SIZE][MAZE_SIZE];  // = {0}; // 2D array storing cell where there are shortcuts

// ####################################################################################
// ####################################################################################
// ##################################   MAIN FILE   ###################################
// ####################################################################################
// ####################################################################################

void setup() {
  // Arduino Pin Initialisation
  Serial.begin(BAUDRATE);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);

  pinMode(s0_color, OUTPUT);
  pinMode(s1_color, OUTPUT);
  pinMode(s2_color, OUTPUT);
  pinMode(s3_color, OUTPUT);
  pinMode(out_color, INPUT);
  digitalWrite(s0_color, HIGH);
  digitalWrite(s1_color, HIGH);

  pinMode(rTrig, OUTPUT);
  pinMode(rEcho, INPUT);
  pinMode(lTrig, OUTPUT);
  pinMode(lEcho, INPUT);
  pinMode(fTrig, OUTPUT);
  pinMode(fEcho, INPUT);

  // Variables
  current_dir = START_DIR;
  current_xcoor = START_X;
  current_ycoor = START_Y;
  current_color = START_COLOR;

  // Coordinate System (x,y):
  //    y1    y2    y3    y4
  // x1 (1,1) (1,2) (1,3) (1,4)
  // x2 (2,1) (2,2) (2,3) (2,4)
  // x3 (3,1) (3,2) (3,3) (3,4)
  // x4 (4,1) (4,2) (4,3) (4,4)

  // Initialising Maze Array with 0
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j] = -1;
      shortcut_loca[i][j] = 0;
    }
  }
  maze[START_X][START_Y] = 0;

  Serial.println("Start");
  while(Serial.read() != 's') {} // wait
}


void loop() {
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //  1. Localisation and Cell Switch Detection
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    delay(5000);
    // Printing Debugger
    Serial.println("------------------------------------");
    printCoordinates();
    Serial.println("moveForward");
    moveForward();
    
    // First check if the coordinates have changed since the last check.
    // Every time, we switch cell.
    if (determineColor(0) != current_color)  
    {
        Serial.println("------------------NEW CELL------------------");
        digitalWrite(19, HIGH);
        // During this time it has no adjustment.
        delay(1500); // Delay speed to go forward to centre of the block.
        digitalWrite(19, LOW);
        stop();
        current_color = determineColor(1);                  // Update current colour, to the new colour detected, and print new colour.
        Serial.println("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~");   // Print new line.

        // Update the current coordinate based on the current direction.
        if (current_dir == 0){
            Serial.println("Adding y");
            current_ycoor += 1;
        }
        else if (current_dir == 90){
            Serial.println("Subtracting x");
            current_xcoor -= 1;
        }
        else if (current_dir == 180){
            Serial.println("Substracting y");
            current_ycoor -= 1;
        }
        else if (current_dir == 270) {
            Serial.println("Adding x");
            current_xcoor += 1;
        }
        else if (current_dir == 360) {
            current_dir = 0;
            current_ycoor += 1;
        }

        // If the current cell is not new then make the current distance the same as the current cell distance.
        if (maze[current_xcoor][current_ycoor] != -1) {
            current_dist = maze[current_xcoor][current_ycoor];
        } 
        else {
            // Otherwise, we incrementing the current distance by 1.
            current_dist += 1;
        }
        maze[current_xcoor][current_ycoor] = current_dist;
        printMaze();
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  2.  Plan Next Step & Orientate
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      int index = findNextCell();
      Serial.print("Planning Index: ");
      Serial.println(index);
      switch (index) {
         case 0: rotateLeft(); moveForward(); break;   // Left
         case 1: moveForward(); break;                 // Forward
         case 2: rotateRight(); moveForward(); break;  // Right
         case 3: rotate180(); moveForward(); break;    // Dead-End, Turn Around
         }
    }
}

int determineColor(int printing_or_not) {
    digitalWrite(s2_color, LOW);
    digitalWrite(s3_color, LOW);
    int red = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s3_color, HIGH);
    int blue = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s2_color, HIGH);
    int green = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    //  COLOR --------------------------------------------------------

    int returnvalue;

    red = map(red, cal_min_r, cal_max_r, 255, 0);
    green = map(green, cal_min_g, cal_max_g, 255, 0);
    blue = map(blue, cal_min_b, cal_max_b, 255, 0);

    if (printing_or_not) {
        Serial.print("R: "); Serial.print(red);
        Serial.print(" --- G: "); Serial.print(green);
        Serial.print(" ---B: "); Serial.print(blue);
    }

    // determine color:
    if (red < black_threshold && blue < black_threshold && green < black_threshold ){
        Serial.println("Black");
        return 0;
    } else if (red > white_threshold && blue > white_threshold && green > white_threshold){
        Serial.print("White");
        return 1;
    } else{
        Serial.println("Red");
        return 2;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONS FOR ULTRASONIC SENSOR
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//float ultra(int trigPin, int echoPin) {
//    long duration;
//    int distance;
//
//    // Clears the trigPin.
//    digitalWrite(trigPin, LOW);
//    delayMicroseconds(2);
//    
//    // Sets the trigPin on HIGH state for 10 micro seconds.
//    digitalWrite(trigPin, HIGH);
//    delayMicroseconds(10);
//    digitalWrite(trigPin, LOW);
//    
//    // Reads the echoPin, returns the sound wave travel time in microseconds.
//    duration = pulseIn(echoPin, HIGH);
//
//    // Calculating the distance in cm.
//    distance = duration * 0.034 / 2; 
//
//    printUltra(distance, trigPin, echoPin);
//
//    return distance;
//}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONS FOR NAVIGATION
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int findNextCell()  // return the index
{
    String inputString = findFreeCells(); // Holds the cell detail of which cell is accessible from current position.
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
    // Planning index format (0 means free cell left, 1 means free cell front, 2 means free cell right)
    // Need to consider what is there are multiple free cell????
}

String findFreeCells() {
    // A function supposed to get the readings from the three ultrasonic sensors and compare it with a threshold value to determine whether there is a wall or free space in each direction
    // This function outputs a 1x3 array, with values of 0 and 1 for each element, 1 for availabel (no Wall) and 0 for unavaialble(Wall), the three values correspond to [left,front,right]
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONS FOR MOTORS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~a
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void stop() {
  //setting the direction of both motors as forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(ena, 0);
    analogWrite(enb, 0);
}

void moveForward() {
    //setting the direction of both motors as forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    adjustSpeed(); //love u
    
    //setting the speed of both motors
    analogWrite(enb, speed_right);
    analogWrite(ena, speed_left);
    
    delay(adjust_rate);
}

void rotateLeft() {

    //setting the direction of motor a as forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    //setting the direction of motor b as backward 
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
    analogWrite(ena, speed);
    analogWrite(enb, speed);
    // Update global variables;
    current_dir += 90;
    current_dir = validateDir(current_dir);

    delay(700);  //Stops after 100ms MAYBE NOT NEEDED
    stop();
}

void rotateRight() {
    //setting the direction of motor a as forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    //setting the direction of motor b as backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    //setting the speed of both motors// might resort to just making it 255 to turn as fast as possible
    analogWrite(ena, speed);
    analogWrite(enb, speed);
    // Update global variables;
    current_dir -= 90;
    current_dir = validateDir(current_dir);
    delay(700);  //Stops after 100ms MAYBE NOT NEEDED
    stop();
}
void moveBackward() {
  //setting the direction of both motors as backward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  //setting the speed of both motors
  analogWrite(ena, speed);  //  speed has to be between 0 and 255
  analogWrite(enb, speed);
}

void rotate180() {
  rotateRight();
  rotateRight();
}

int validateDir(int dir) {
  if (dir > 360) return dir - 360;
  else if (dir < 0) return dir + 360;
  else return dir;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FUNCTIONS FOR PRINTING
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printCoordinates(){
  // Print out the coordinates.
    Serial.print("x:");
    Serial.print(current_xcoor);
    Serial.print(", ");
    Serial.print("y:");
    Serial.print(current_ycoor);
    Serial.print(", ");
    Serial.print("Current Direction:");
    Serial.print(current_dir);
    Serial.println(" ");
}

void printMaze(){
  for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            Serial.print(maze[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
}

void printUltra(int distance, int trig, int echo){
    if (trig == 11 and echo == 5){
        Serial.print("Left:");
        Serial.println(distance); 
    }
    else if (trig == 10 and echo == 3){
        Serial.print("Right:");
        Serial.println(distance);
    }
    else if (trig == 13 and echo == 12){
        Serial.print("Front:");
        Serial.println(distance);
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MOTOR CONTROL
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void adjustSpeed(){
    int dif_dist;
    int adjust_val, left_adjust, right_adjust;
    int s;

    // Get the distance reading from left and riht ultra
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // note: use integer first! - might need to change for more accuracy
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float left_dist = ultra(lTrig, lEcho);
    float right_dist = ultra(rTrig, rEcho);
    // Serial.print("Left: ");     Serial.print(left_dist);
    // Serial.print("; Right: ");  Serial.print(right_dist);
    
    // Compute the adjusting distance
    dif_dist = abs(left_dist - right_dist);

    // Compute the adjust value
    if (dif_dist > dif_threshold ){
        s = scaling_factor_far;
        Serial.println(" --> DIST IS FAR\n");
    } else{
        s = scaling_factor;
        Serial.println("...");
    }
    adjust_val = s * dif_dist;
    
    if (left_dist > right_dist){        // need to steer left --> decrease left 
        left_adjust     = - adjust_val;
        right_adjust    = adjust_val;
    } else{                             // need to steer right --> decrease right
        left_adjust     = adjust_val;
        right_adjust    = - adjust_val;
    }

    // Update the speed
    speed_left= speed_normal + left_adjust;
    speed_right = speed_normal + right_adjust;

    // TUNIGGGGGGGGGGGGGGGG :(
}

// float ultra_shit(int trigPin, int echoPin) {
//   int flag = 1;
  
//   int prev_dist = ultra_shit(trigPin, echoPin);
//   delay(100);
//   while (flag == 1){
//     int tmp = ultra_shit(trigPin, echoPin);
//     if (abs(tmp - prev_dist) > 20 or tmp == 0){
//       continue;
//     }
//     else {
//       printUltra(tmp, trigPin, echoPin);
//       return tmp;
//     }
//   }
// }

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
    printUltra(distance, trigPin, echoPin);
    
    delayMicroseconds(50);
  
    return distance;
}