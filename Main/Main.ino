// --------------------------------------------------------------------------
// LIBRARIES
// --------------------------------------------------------------------------
#include <libMazePrint.h>
#include <libMazeUltra.h>
#include <libMazeMotor.h>
#include <libMazeColour.h>
#include <libMazeNavigate.h>

// --------------------------------------------------------------------------
// MAZE CONSTRAINTS
// --------------------------------------------------------------------------
#define MAZE_SIZE 4
#define BAUDRATE 9600

// --------------------------------------------------------------------------
// PIN DECLARATIONS
// --------------------------------------------------------------------------
#define led 19

#define in3 2        // LEFT MOTOR
#define in4 4        // LEFT MOTOR
#define enb 9        // LEFT MOTOR

#define in1 7        // RIGHT MOTOR
#define in2 8        // RIGHT MOTOR
#define ena 6        // RIGHT MOTOR

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

// --------------------------------------------------------------------------
// COLOUR SENSOR CALIBRATION CONSTANTS
// --------------------------------------------------------------------------
// Constant
#define white_threshold 200
#define red_threshold 230
#define black_threshold 70
// Calibrated constant
// WHITE ------------
#define cal_min_r 5
#define cal_min_g 5
#define cal_min_b 5
#define cal_max_r 46
#define cal_max_g 51
#define cal_max_b 41

const int all_COLORS[] = {0, 1, 2, 3, 4, 5, 6}; // [B, W, R, b, g, p, y]
const int SPECIAL_COLORS[] = {3, 4, 5, 6};
const int GATE_DELAY = 3000;

// Maze Parameters
int finalDistance = 11;

// --------------------------------------------------------------------------
// MOTOR CONTROL CONSTANTS
// --------------------------------------------------------------------------
const int speed = 150;

// adjust speed related
int speed_normal = 100;
int speed_right = speed_normal;
int speed_left = speed_normal;
int adjust_rate = 200;
int scaling_factor = 2;
int scaling_factor_far = 4;
int dif_threshold = 7;
int speed_offset = 15;

int turn_90_time = 1000; // 700 - previous value from COMP
int centering_time = 5500;
int log_time = 1000;

//unsigned long previous_millis_adjust_speed = 0;
//unsigned long current_millis_adjust_speed = 0;
//
//unsigned long previous_millis_show_log = 0;
//unsigned long current_millis_show_log = 0;

// --------------------------------------------------------------------------
// MAZE, DIRECTION AND MOVEMENT CONSTANTS
// --------------------------------------------------------------------------
const int START_X = 3;
const int START_Y = 3;
const int START_DIR = 90;
const int START_COLOR = 2;

// CHANGE TO CONST Ryan!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int xCoor = 3; // Column
int yCoor = 2; // Row

int current_dir;
int current_xcoor;
int current_ycoor;
int current_color; //already init by Ryan
int current_dist;

int backFlag = 0;

int maze[MAZE_SIZE][MAZE_SIZE];           // Initialised with -1 meaning unexplored; and, used to store the distance from the start in each cell.
int shortcut_loca[MAZE_SIZE][MAZE_SIZE];  // Initialised with 0 with value 1 signifying a shortcut available at that cell.
unsigned long startTime;                  // Time constants for activation maniplations.


// --------------------------------------------------------------------------
//                    |\    /|      /\       |    |\    |
//                    | \  / |     /  \      |    | \   |
//                    |  \/  |    /____\     |    |  \  |
//                    |      |   /      \    |    |   \ |
//                    |      |  /        \   |    |    \|
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// SETUP
// --------------------------------------------------------------------------
void setup() {
  Serial.begin(BAUDRATE);

  // Pin Setup
  motorPinSetup();
  colourPinSetup();
  ultraPinSetup();
  // Maze & Direction Setup
  startDirectionSetup();
  initialiseMaze();

  Serial.println("Welcome to the best maze solver\n");
  Serial.println(">> Press ENTER to start\n\n");
  while (Serial.read() != '\n') {} // wait
  startTime = millis();
}


// --------------------------------------------------------------------------
// LOOP
// --------------------------------------------------------------------------
void loop() {
  if ( not backFlag) {
    // --------------------------------------------------------------------------
    // 1. LOCALISATION & CELL SWITCHING DETECTION
    // --------------------------------------------------------------------------
    // Serial print data every loop for visualisation.
    //    Serial.println("---<NEW LOOP>---");

    // At the start, we allow the robot to keep moving straight; hence, always positioned the robot in the direction to the next cell.
    moveForward(0);

    //  int distances[3];
    //  for (int i = 0; i < 3; i++) {
    //    distances[i] = ultra(TrigPins[i], EchoPins[i]);
    //    printUltra(distances[i], TrigPins[i], EchoPins[i]);
    //  }

    // Check 1: When entering a new cell, i.e. colour changes.
    int new_color = determineColor(0);
    if (new_color != current_color)
    {
      Serial.println("===========================================================");
      // Update the current colour and current coordinate when entering the new cell.
      stop(in1, in2, in3, in4, ena, enb);
      Serial.println("---<COLOUR & COORDINATE UPDATED>---");
      printCoordinates(current_xcoor, current_ycoor, current_dir);
      //    current_color = determineColor(1, s2_color, s3_color, out_color, cal_min_r, cal_max_r, cal_min_g, cal_max_g, cal_min_b, cal_max_b, black_threshold, white_threshold);
      current_color = new_color;
      current_color = determineColor(0);
      Serial.print("Update new color: ");
      Serial.println(current_color);
      updateCoordinate();

      // Timing block that allows thfe robot to centre itself when entering a new cell, without using delay, to allow adjustSpeed() to keep running.
      digitalWrite(led, HIGH); // Turn LED on to signify, we have entered a new cell.
      Serial.println("\n---<ENTERED NEW CELL>---");
      printMaze();
      delay(log_time); // Delay for user process.

      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // Center the robot once we have entered a new cell.
      Serial.println("\n---<CENTERING ROBOT>---");
      unsigned long currentTime = millis();
      unsigned long elapsedTime = 0;
      startTime = millis();
      while (elapsedTime < centering_time) { // 2300ms
        //            stop(in1, in2, in3, in4, ena, enb);
        moveForward(1);
        currentTime = millis();
        elapsedTime = currentTime - startTime;
        if (elapsedTime % 1500 == 0) Serial.print("Centering ...");
      }
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      digitalWrite(led, LOW); // Stop LED to signify, we have centered our robot.
      stop(in1, in2, in3, in4, ena, enb);

      // If the current cell is not new then make the current distance the same as the current cell distance.
      Serial.println("\n---<DISTANCE FROM ORIGIN UPDATED>---");
      if (maze[current_xcoor][current_ycoor] != -1) {
        current_dist = maze[current_xcoor][current_ycoor];
      }
      else {
        // Otherwise, we incrementing the current distance by 1.
        current_dist += 1;
      }
      maze[current_xcoor][current_ycoor] = current_dist;
      delay(log_time);

      // --------------------------------------------------------------------------
      // 2. PATH PLANNING & ORIENTATION ADJUSTMENT
      // --------------------------------------------------------------------------
      // Using the ultrasonic sensor, find what cell is available; and out of these avaiable cell, what cell should we go to next.
      int index = findNextCell(lTrig, fTrig, rTrig, lEcho, fEcho, rEcho); // Index determines what direction to move to next during discovery.
      Serial.println("---<PATH PLAN NEXT>---");
      Serial.print("Planning Index: ");
      Serial.println(index);
      delay(log_time);
      //    current_millis_show_log = millis();
      //    if (current_millis_show_log - previous_millis_show_log > 1000) {
      //      previous_millis_show_log = current_millis_show_log;

      // Determine what action to take depending on the index.
      if (index == 0) {
        // Rotate in-place left 90 degrees and move into the next cell.
        rotateLeft(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
        // Update global variables;
        current_dir += 90;
        current_dir = validateDir(current_dir);
        stop(in1, in2, in3, in4, ena, enb);
        moveForward(1);
      }
      else if (index == 1) {
        // Continue moving forward.
        moveForward(1);
      }
      else if (index == 2) {
        // Rotate in-place right 90 degrees and move into the next cell.
        rotateRight(speed, turn_90_time, in1, in2, in3, in4, ena, enb);
        // Update global variables;
        current_dir -= 90;
        current_dir = validateDir(current_dir);
      }
      else if (index == 3) {
        // Dead end, rotate 180 degrees and return to the previous cell.
        rotate180(speed, turn_90_time, in1, in2, in3, in4, ena, enb);
        // Update global variables;
        current_dir += 180;
        current_dir = validateDir(current_dir);
        stop(in1, in2, in3, in4, ena, enb);
        moveForward(1);
      }
    }

//    Serial.print(current_xcoor);
//    Serial.println(xCoor);
    if (current_xcoor == xCoor and current_ycoor == yCoor) {
      backFlag = 1;
    }

  }
  else if (backFlag == 1) {
    // --------------------------------------------------------------------------
    // 3. BACKTRACKING & FINDING ROUTE HOME
    // --------------------------------------------------------------------------

    Serial.println("---<BACKTRACK TO HOME>---");
    if (finalDistance != 0) {
      if (determineColor(0) != current_color) {
        moveForward(1);
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Center the robot once we have entered a new cell.
        Serial.println("\n---<CENTERING ROBOT>---");
        unsigned long currentTime = millis();
        unsigned long elapsedTime = 0;
        startTime = millis();
        while (elapsedTime < centering_time) { // 2300ms
          //            stop(in1, in2, in3, in4, ena, enb);
          moveForward(1);
          currentTime = millis();
          elapsedTime = currentTime - startTime;
          if (elapsedTime % 1500 == 0) Serial.print("Centering ...");
          // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

          // Checking maze info and rotate
          if (current_dir == 0) {
            if ((yCoor != 0) and (maze[yCoor - 1][xCoor] == finalDistance - 1)) {
              Serial.println("rotateLeft()");
              rotateLeft(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir += 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
            else if ((yCoor != 4) and (maze[xCoor][yCoor + 1] == finalDistance - 1)) {
              Serial.println("rotateRight()");
              rotateRight(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir -= 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
          }
          else if (current_dir == 90) {
            if ((xCoor != 0) and (maze[xCoor - 1][yCoor] == finalDistance - 1)) {
              Serial.println("rotateLeft()");
              rotateLeft(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir += 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
            else if ((yCoor != 4) and (maze[xCoor + 1][yCoor] == finalDistance - 1)) {
              Serial.println("rotateRight()");
              rotateRight(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir -= 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
          }
          else if (current_dir == 180) {
            if ((yCoor != 4) and (maze[xCoor][yCoor + 1] == finalDistance - 1)) {
              Serial.println("rotateLeft()");
              rotateLeft(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir += 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
            else if ((yCoor != 0) and (maze[xCoor][yCoor - 1] == finalDistance - 1)) {
              Serial.println("rotateRight()");
              rotateRight(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir -= 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
          }
          else if (current_dir == 270) {
            if ((xCoor != 4) and (maze[xCoor + 1][yCoor] == finalDistance - 1)) {
              Serial.println("rotateLeft()");
              rotateLeft(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir += 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
            else if ((xCoor != 0) and (maze[xCoor - 1][yCoor] == finalDistance - 1)) {
              Serial.println("rotateRight()");
              rotateRight(speed, turn_90_time, in1, in2, in3, in4, ena, enb );
              // Update global variables;
              current_dir -= 90;
              current_dir = validateDir(current_dir);
              delay(log_time);
            }
          }
          finalDistance = finalDistance - 1;
        }
      }
    }
  }
}


// --------------------------------------------------------------------------
//                  ~~~~~~~   |      |   |\    |   ~~~~~~~
//                  |         |      |   | \   |   |
//                  |______   |      |   |  \  |   |
//                  |         |      |   |   \ |   |
//                  |         |______|   |    \|   |______
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//                 FUNCTION REQUIRING GLOBAL VARIABLE SCOPE
// --------------------------------------------------------------------------

void moveForward(int adjustOn) {
  //setting the direction of both motors as forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  if (adjustOn) {
    adjustSpeed(); //love u
  }

  //setting the speed of both motors
  analogWrite(enb, speed_right);
  analogWrite(ena, speed_left);
}

void adjustSpeed() {
  int dif_dist;
  int adjust_val, left_adjust, right_adjust;
  int s;


  // Get the distance reading from left and riht ultra
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // note: using integer for now - might need to change for more accuracy
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  float left_dist = ultra(lTrig, lEcho);
  float right_dist = ultra(rTrig, rEcho);

  //  current_millis_adjust_speed = millis();
  //  if (current_millis_adjust_speed - previous_millis_adjust_speed >= adjust_rate) {
  //    previous_millis_adjust_speed = current_millis_adjust_speed;

  if (left_dist >= 30) {
    Serial.println("Hard-code LEFT sensor to 12");
    left_dist = 12;
  }
  if (right_dist >= 30) {
    Serial.println("Hard-code RIGHT sensor to 12");
    right_dist = 12;
  }
  // Serial.print("Left: ");     Serial.print(left_dist);
  // Serial.print("; Right: ");  Serial.print(right_dist);

  // Compute the adjusting distance
  dif_dist = abs(left_dist - right_dist);

  // Compute the adjust value
  if (dif_dist > dif_threshold ) {
    s = scaling_factor_far;
    //Serial.println(" --> DIST IS FAR\n");
  } else {
    s = scaling_factor;
    //Serial.println("...");
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
  //  }
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
    Serial.print("R: "); Serial.print(red);
    Serial.print(" --- G: "); Serial.print(green);
    Serial.print(" ---B: "); Serial.print(blue);
    Serial.print("-->\t");
  }

  // Version 3 - Sep 17
  // determine color:
  if (red > red_threshold && blue < red_threshold && green < red_threshold ) {
    if (printing_or_not) Serial.println("Red");
    return 2;
  } else if (red > white_threshold && blue > white_threshold && green > white_threshold) {
    if (printing_or_not) Serial.println("White");
    return 1;
  } else {
    if (printing_or_not) Serial.println("Black");
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//////////////        A      MAZE     ING             //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void updateCoordinate() {
  // Update the current coordinate based on the current direction.
  if (current_dir == 0) {
    Serial.println("Adding y");
    current_ycoor += 1;
  }
  else if (current_dir == 90) {
    Serial.println("Subtracting x");
    current_xcoor -= 1;
  }
  else if (current_dir == 180) {
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
}
void startDirectionSetup() {
  current_dir = START_DIR;
  current_xcoor = START_X;
  current_ycoor = START_Y;
  current_color = START_COLOR;
}

void initialiseMaze() {
  // Coordinate System (x,y):
  //      y1    y2    y3    y4
  // x1 (1,1) (1,2) (1,3) (1,4)
  // x2 (2,1) (2,2) (2,3) (2,4)
  // x3 (3,1) (3,2) (3,3) (3,4)
  // x4 (4,1) (4,2) (4,3) (4,4)
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j] = -1;
      shortcut_loca[i][j] = 0;
    }
  }
  maze[START_X][START_Y] = 0;
}

void printMaze() {
  // Function to print the whole maze (for visualisation).
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      Serial.print(maze[i][j]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////         Set up - init       ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void ultraPinSetup() {
  pinMode(rTrig, OUTPUT); pinMode(rEcho, INPUT); pinMode(lTrig, OUTPUT); pinMode(lEcho, INPUT); pinMode(fTrig, OUTPUT); pinMode(fEcho, INPUT);
}
void motorPinSetup() {
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT); pinMode(in1, OUTPUT); pinMode(in2, OUTPUT); pinMode(ena, OUTPUT); pinMode(enb, OUTPUT);
}
void colourPinSetup() {
  pinMode(s0_color, OUTPUT); pinMode(s1_color, OUTPUT); pinMode(s2_color, OUTPUT); pinMode(s3_color, OUTPUT); pinMode(out_color, INPUT);
  digitalWrite(s0_color, HIGH); digitalWrite(s1_color, HIGH);
}
