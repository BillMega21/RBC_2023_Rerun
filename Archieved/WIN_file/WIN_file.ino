// Pin declare ___________________________________
#define RX 3
#define TX 2
#define LED_PIN 14
// MOTOR ___________________________________
// left
#define in3 2
#define in4 4
#define enb 5
// right
#define in1 7
#define in2 8
#define ena 3
// color ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define s0_color 14
#define s1_color 15
#define s2_color 16
#define s3_color 17
#define out_color 18
// Calibrated constant
// WHITE ------------
#define cal_min_r 6
#define cal_min_g 6
#define cal_min_b 5
// BLACK ------------
#define cal_max_r 42
#define cal_max_g 48
#define cal_max_b 39
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ultrasonic
#define rTrig 13
#define rEcho 12
#define lTrig 10
#define lEcho 5
#define fTrig 11
#define fEcho 3
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MAZE_SIZE 4
#define BAUDRATE 9600


// Constant
const int START_X = 3;
const int START_Y = 3;
const int START_DIR = 0;
const int START_COLOR = 2;
const int TrigPins[3] = { lTrig, fTrig, rTrig };
const int EchoPins[3] = { lEcho, fEcho, rEcho };

const int speed = 40;

//const int MAZE_SIZE = 4;
const int all_COLORS[] = { 0, 1, 2, 3, 4, 5, 6 }; // [B, W, R, b, g, p, y]
const int SPECIAL_COLORS[] = { 3, 4, 5, 6 };
const int GATE_DELAY = 3000;

// GLOBAL VARIABLE
int current_dir;
int current_xcoor;
int current_ycoor;
char current_color;
int current_dist;
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
// #####################	MAIN FILE		###########################################
// ####################################################################################
// ####################################################################################


void setup() {
  // Arduino init
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

  // Variables

  current_dir = START_DIR;
  current_xcoor = START_X;
  current_ycoor = START_Y;
  current_color = START_COLOR;

  // Init the array:
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j] = -1;
      shortcut_loca[i][j] = 0;
    }
  }
}


void loop() {

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //  1.
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   moveForward();
  delay(2000);
  // rotateLeft(in1, in2, ena, in3, in4, enb, speed);
  // delay(2000);
  // rotateRight(in1, in2, ena, in3, in4, enb, speed);

    // PRINT out the Coordinate
    Serial.print("x:");
    Serial.print(current_xcoor);
    Serial.print(" ");
    Serial.print("y:");
    Serial.print(current_ycoor);
    Serial.print(" ");
    Serial.print("");
    Serial.print(current_dir);
    Serial.println(" ");
    Serial.println("");

    if (determineColor(0) != current_color)  // First check if the coordinates have changed since the last check
    {
        stop();
        //	- update current color
        current_color = determineColor(1);
        Serial.println("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        //    - update current coordinate
        if (current_dir = 0){
            Serial.println("Adding x");
            current_xcoor += 1;
        }
        else if (current_dir = 90){
            Serial.println("Adding y");
            current_ycoor += 1;
        }
        else if  (current_dir = 180){
            Serial.println("Substracting x");
            current_xcoor -= 1;
        }
        else {
            Serial.println("Substracting y");
            current_ycoor = -1;
        }

        if (maze[current_xcoor, current_ycoor] != -1) {
            current_dist = maze[current_xcoor, current_ycoor];  // make no change
        } else {
            current_dist += 1;
        }

    //	- update explored cells array
    // PRINT 2D array ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    maze[current_xcoor][current_ycoor] = current_dist;
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
        Serial.print(maze[i][j]);
        Serial.print(" ");
        }
        Serial.print("\n");
    }
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  2. 	Plan_Next_step & orientate
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int index = findNextCell();

    Serial.print("Planning index: ");
    Serial.println(index);
    //   switch (index) {
    //     case 0: rotateLeft(); break;   //Left
    //     case 1: break;                 // Forward
    //     case 2: rotateRight(); break;  // Right
    //     case 3: rotate180(); break;    // DEAD-END, turn around
    //   }

}


int determineColor(int printing_or_not) {

    // Get color ----------------------------------------------------------------
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
    Serial.print("R: ");
    Serial.print(red);
    Serial.print(" - G: ");
    Serial.print(green);
    Serial.print(" -B: ");
    Serial.print(blue);
    Serial.print("----> ");
    }

    // determine color:
    int purple = (red + blue) / 2;
    int threshold = 170; int black_threshold = 40;

    delay(1000);

    if (red > threshold && green > threshold && blue > threshold) {
        return 1;  // white
    } else if (red < black_threshold && blue < black_threshold && green < black_threshold) {
    return 0;  // black
    } else if (red > 180 && green > threshold) {
    return 4;  // green
    } else if (red > threshold && green > threshold) {
    return 6;  // yellow
    }
    // else if (green > threshold){
    //     return 4;
    // }
    else if (red > threshold) {
    return 2;  // red
    } else if (blue > 110) {
    return 3;  // blue
    } else {
    return 5;  // purple
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ULTRASONIC 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float ultra(int trigPin, int echoPin) {
    long duration;
    int distance;

    // Clears the trigPin.
    digitalWrite(trigPin, LOW);  //////////////I AM NOT SURE IF CAN USE 0 INSTEAD OF LOW
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds.
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds.
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance.
    distance = duration * 0.034 / 2;  //output as cm

    return distance;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONNNNNNNNNNNNNN for NAVIGATION
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int findNextCell()  // return the index
{
  int* FreeCells = findFreeCells();
  int index = 0;
  for (int i = 0; i < 2; i++) {
    if (FreeCells[i] == 1) {
      return index;
    }
    index++;
  }
  return index;
}

int findFreeCells() {
    // A function supposed to get the readings from the three ultrasonic sensors and compare it with a threshold value to determine whether there is a wall or free space in each direction
    // This function outputs a 1x3 array, with values of 0 and 1 for each element, 1 for availabel (no Wall) and 0 for unavaialble(Wall), the three values correspond to [left,front,right]
    float threshold = 40.0;
    //int TrigPins[3] = {lTrig,fTrig,rTrig};
    //int EchoPins[3] = {lEcho,fEcho,rEcho};
    float distances[3];

    for (int i = 0; i < 3; i++){
        distances[i] = ultra(TrigPins, EchoPins);
    }

    int availability[3];
    for (int i = 0; i < 2; i++) {
        if (distances[i] > threshold) {
            availability[i] = 1;
        } else {
            availability[i] = 0;
        }
    }
    return availability;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONNNNNNNNNNNNNN for MOTOR
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void stop() {
  analogWrite(ena, 0);
  analogWrite(enb, 0);
}

void moveForward() {
    //setting the direction of both motors as forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    //setting the speed of both motors
    analogWrite(ena, speed);  //  speed has to be between 0 and 255
    analogWrite(enb, speed);
    delay(100);  //Stops after 100ms MAYBE NOT NEEDED
    stop();
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

    delay(375);  //Stops after 100ms MAYBE NOT NEEDED
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
    delay(375);  //Stops after 100ms MAYBE NOT NEEDED
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
