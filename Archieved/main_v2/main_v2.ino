
// Pin
#define RX 3
#define TX 2
#define LED_PIN 14
#define in1 4
#define in2 5
#define ena 6
#define in3 7
#define in4 8
#define enb 9

#define rTrig 13
#define rEcho 12
#define fTrig 11
#define fEcho 5
#define lTrig 10
#define lEcho 3


#define MAZE_SIZE 4
#define BAUDRATE 9600

// 0 black, 1 white, 2 red, 3 4 5 6 - blue green purple yellow
/////////////////////////////////////////////////////////////////////////////////

// Constant
const int START_X = 3;
const int START_Y = 3;
const int START_DIR = 0;
const int START_COLOR = 2;
const int TrigPins[3] = { lTrig, fTrig, rTrig };
const int EchoPins[3] = { lEcho, fEcho, rEcho };

//const int MAZE_SIZE = 4;
const int all_COLORS[] = { 0, 1, 2, 3, 4, 5, 6 };
/////////////////////////[B,W,R,b,g,p,y]
const int SPECIAL_COLORS[] = { 3, 4, 5, 6 };
const int GATE_DELAY = 3000;

// GLOBAL VARIABLE
int current_dir;
int current_xcoor;
int current_ycoor;
char current_color;
int new_line = 1;  // For bluetooth

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
  pinMode(1, OUTPUT);

  // Variable init
  //maze[current_xcoor][current_ycoor] = 0; // Set starting distance to 0

  current_dir = START_DIR;
  current_xcoor = START_X;
  current_ycoor = START_Y;
  current_color = START_COLOR;

  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j] = -1;
      shortcut_loca[i][j] = 0;
    }
  }
}
void stop(int enA, int enB) {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}
void moveForward(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed) {
  //setting the direction of both motors as forward
  digitalWrite(inA_1, LOW);
  digitalWrite(inA_2, HIGH);
  digitalWrite(inB_1, LOW);
  digitalWrite(inB_2, HIGH);

  //setting the speed of both motors
  analogWrite(enA, speed);  //  speed has to be between 0 and 255
  analogWrite(enB, speed);
  delay(100);  //Stops after 100ms MAYBE NOT NEEDED
  stop(enA, enB);

  void rotateLeft(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed);
  void rotateRight(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed);
  void rotate180(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed);

  int validateDir(int dir);
  int determineColor();
  int findNextCell();
  float* all_ultra(int trigPins[3], int echoPins[3]);
}
void blinkGoal(int LED_pin);


void loop() {

  ////////////////////////////////////////////////////////////////
  // Localisation and Mapping
  ///////////////////////////////////////////////////////////////

  int cells_checked = 0;  // numbers of cells explored
  int current_dist = 0;   // the current distance from the most recent explored cell to the start

  while (current_color != 'r' && current_dist != 0) {

    moveForward(in1, in2, ena, in3, in4, enb, 255);  // Move "unlimited" till reach new color (still need define time control inside)

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  1. Identify current state()
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (determineColor() != current_color)  // First check if the coordinates have changed since the last check
    {
      //	- update current color
      current_color = determineColor();

      //    - update current coordinate
      current_xcoor += cos(current_dir);  // x-axis is vertical axis
      current_ycoor += sin(current_dir);

      //	- update current distance
      if (maze[current_xcoor, current_ycoor] != -1)  //if cell been explored before
      {
        current_dist = maze[current_xcoor, current_ycoor];  // make no change
      } else {
        current_dist += 1;

        //	- update explored cells array
        maze[current_xcoor][current_ycoor] = current_dist;

        //	- update special cells array
        for (int i; i < 4; i++)  //Go through the special colors array and compare it with current color
        {
          int color = SPECIAL_COLORS[i];
          if (current_color == color) {
            shortcut_loca[current_xcoor][current_ycoor] = current_color;  //Update the special cells array with the current coordinates
            break;
          }
        }
      }
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      //  2. 	Plan_Next_step & orientate
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      /*
      	- Use ultrasonic to find free path
        - Direct robot to free path
        	- Not to go to the path already been there --> check the CELL we about to go to
            - ALWAYS turn left
            	- meet dead end, return to intersection
        
          * */
      int index = findNextCell();
      switch (index) {
        case 0: rotateLeft(in1, in2, ena, in3, in4, enb, 255); break;   //Left
        case 1: break;                                                  // Forward
        case 2: rotateRight(in1, in2, ena, in3, in4, enb, 255); break;  // Right
        case 3: rotate180(in1, in2, ena, in3, in4, enb, 255); break;    // DEAD-END, turn around
      }
    }
    // ENDING THE WHILE LOOP HEREE - REACH THE GOAL
  }

  // 2. Flash the LED 3 time, 1-1s
  blinkGoal(LED_PIN);

  // 3. Compute the best shortcut, Backtrack
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find optimal back track
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Search for index of shortcut_loca
  int shortcut_pair_color[4][2] = { 0 };  // Array to store each pair - maximum of 4 pairs for 4 special colors
                                          // Follow the order of SPECIAL_COLORS[] = {'b','g','p','y'};
  int color_idx = -1;                     // To store first dimension index for the array above


  // Iterate through the shortcut_loca array, find the index, put that in shortcut_pair_color
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      int special_color = shortcut_loca[i][j];
      // Get distance
      int tmp_distance = maze[i][j];

      // If found non-zero value - mean special colors at that CELL:
      if (special_color == 0 & tmp_distance == -1) {

        // DEAL WITH THE SCENARIO WHERE THERE ARE HIDDEN SHORTCUT ALONG THE WAY
        // get all adajcent sqaures
        // check if there is only one unexplored adjecent squares, if yes then it is the other sqaure
        // if not then we cannot conclud

        continue;
        // literally, to do this, we have to go through the shortcut_pair_color array <-- MAYBE LATER, ACCEPT THE ASSUMPTION
        // literally, to do this, we have to go through the shortcut_pair_color array <-- MAYBE LATER, ACCEPT THE ASSUMPTION
        // literally, to do this, we have to go through the shortcut_pair_color array <-- MAYBE LATER, ACCEPT THE ASSUMPTION
        // literally, to do this, we have to go through the shortcut_pair_color array <-- MAYBE LATER, ACCEPT THE ASSUMPTION
        // literally, to do this, we have to go through the shortcut_pair_color array <-- MAYBE LATER, ACCEPT THE ASSUMPTION
      } else if (special_color != 0) {

        // Update into the "pair array"
        switch (special_color) {
          case 3:
            color_idx = 0;
            break;
            break;
          case 4:
            color_idx = 1;
            break;
            break;
          case 5:
            color_idx = 2;
            break;
            break;
          case 6:
            color_idx = 3;
            break;
            break;
          default: break;  // ignore?
        }
        // check if the current color have any previous stored value
        if (shortcut_pair_color[color_idx][0] == 0)
          shortcut_pair_color[color_idx][0] = tmp_distance;
        else shortcut_pair_color[color_idx][1] = tmp_distance;
      }
    }
  }


  // Some LENGTHYYYYYYYY math here, sorry, my brain is almost ded...
  // :((((((((((((((((((((((((((((((()))))))))))))))))))))))))))))))

  // Compute shortest
  int goal_distance = current_dist;  // <--------------------- NEED TO DEFINE THIS FUNCTION
  int distance_to_shortcut = 0;
  int shortcut_gap = 0;
  int pair1, pair2;   // temp value to store "Distance" pair value
  int eval_array[4];  // array to store step values for evaluation

  for (int i = 0; i < 4; i++) {
    pair1 = shortcut_pair_color[i][0];
    pair2 = shortcut_pair_color[i][1];
    // What if the value is 0?
    if (pair1 == 0 || pair2 == 0) continue;
    // Compute:
    shortcut_gap = abs(pair1 - pair2);
    // Find the larger shortcut value (between the pair) to find distance_to_shortcut
    if (pair1 > pair2) distance_to_shortcut = goal_distance - pair1;
    else distance_to_shortcut = goal_distance - pair2;
    // Compute
    eval_array[i] = distance_to_shortcut - shortcut_gap;
  }

  // Find optimal pair to go to (by finding minimum value)
  int min_idx = 0;  // To store index of the minimum value from eval_array, use this to get the optimal shortcut pair
  for (int i = 1; i < 4; i++) {
    if (eval_array[i] < eval_array[min_idx]) {
      min_idx = i;
    }
  }

  // Lengthy math ...
  pair1 = shortcut_pair_color[min_idx][0];  // Get these again to find shortcut_gap
  pair2 = shortcut_pair_color[min_idx][2];
  shortcut_gap = abs(pair1 - pair2);

  // Get the number of steps needed to go to the optimal shortcut CELL:
  int number_steps_to_shortcut = eval_array[min_idx] + shortcut_gap;  // "reverse" the equation to find eval_array above
                                                                      // Get the distance value of the optiamal shortcut CELL <--- use this as the target to BACK TRACK?
  int shortcut_distance = goal_distance - number_steps_to_shortcut;


  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Move - BACK TRACK 	@ We can reuse 1. Identify current state() & 3. Move()
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  rotate180(in1, in2, ena, in3, in4, enb, 255);

  // Go to the optimal SHORTCUT CELL
  while (current_dist != 0) {

    moveForward(in1, in2, ena, in3, in4, enb, 255);  // Move "unlimited" till reach new color (still need define time control inside)

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  1. Identify current state()
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (determineColor() != current_color)  // First check if the coordinates have changed since the last check
    {
      //	- update current color
      current_color = determineColor();
      //    - update current coordinate
      current_xcoor += cos(current_dir);
      current_ycoor += sin(current_dir);
      //	- update current distance
      current_dist -= 1;  // Back track now, use -1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  2. 	Plan_Next_step & orientate
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /*
    	- dont need ultrasonic
        - scan (in the program) adjacent block to find the lower DISTANCE VALUE block        
          * */
    int distance_adj_scan[4] = { 0 };
    if (current_xcoor >= 0 && current_xcoor < MAZE_SIZE && current_ycoor >= 0 && current_ycoor < MAZE_SIZE) {  // ALSO check if it the wall
      // Top
      distance_adj_scan[0] = (current_xcoor < MAZE_SIZE - 1) ? maze[current_xcoor + 1][current_ycoor] : 0;
      // Right
      distance_adj_scan[1] = (current_ycoor < MAZE_SIZE - 1) ? maze[current_xcoor][current_ycoor + 1] : 0;
      // Bottom
      distance_adj_scan[2] = (current_xcoor > 0) ? maze[current_xcoor - 1][current_ycoor] : 0;
      // Left
      distance_adj_scan[3] = (current_ycoor > 0) ? maze[current_xcoor][current_ycoor - 1] : 0;
    } else {
      // Invalid row or column, set all values to 0
      for (int i = 0; i < 4; i++) {
        distance_adj_scan[i] = 0;
      }
    }

    // SCAN FOR THE NEXT LOWER DISTANCE
    int turning_idx = 0;
    for (int i = 0; i < 4; i++) {
      if (distance_adj_scan[i] == current_dist - 1) {
        turning_idx = i;
      }
    }
    // Get the current direction, then find which direction to turn
    int turn_degree = current_dir - (turning_idx * 90);  // <-------- visualisation :)
    turn_degree = validateDir(turn_degree);

    switch (turn_degree) {
      case 0: break;                                                   // No turn
      case 90: rotateRight(in1, in2, ena, in3, in4, enb, 255); break;  // Right
      case 180: rotate180(in1, in2, ena, in3, in4, enb, 255); break;   // Turn around
      case 270: rotateLeft(in1, in2, ena, in3, in4, enb, 255); break;  // Right
      default: break;                                                  //This should never happen unless something is wrong
    }
  }

  // SEND BLUETOOTH SIGNAL using current_color

  //  NAVIGATE THRU THE OPEN GATE !!!!!!!!!!!! <-- do it later, make sure it can actually TRACK BACK

  // end of void loop - GOOD WORK :)
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
  float* distances = all_ultra(TrigPins, EchoPins);
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
// HELPER FUNCTIONNNNNNNNNNNNNN for COLOR
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void calibrate() {
  /*
    //Serial.println("Put black infront");////////////////////////////////////////
    while (Serial.read() != 'c') {} //wait

    color();
    cal_max_r = red; cal_max_g = green; cal_max_b = blue;
    Serial.print("R: ");
    Serial.print(red);
    Serial.print(" --- G: ");
    Serial.print(green);
    Serial.print(" ---B: ");
    Serial.print(blue);
    Serial.println("----> ");

    Serial.println("Put white infront");
    while (Serial.read() != 'c') {} //wait

    color();
    cal_min_r = red; cal_min_g = green; cal_min_b = blue;
    Serial.print("R: ");
    Serial.print(red);
    Serial.print(" --- G: ");
    Serial.print(green);
    Serial.print(" ---B: ");
    Serial.print(blue);
    Serial.println("----> ");

    Serial.println("Done");
    delay(300);
}

  int* color(int s2_pin, int s3_pin) {
  	int *rbg = {0, 0, 0};
  	
    // apply filter for measuring R, B, G values
    digitalWrite(s2_pin, LOW);
    digitalWrite(s3_pin, LOW);
    // RED
    rbg[0] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
    // BLUE
    digitalWrite(s3_pin, HIGH);
    rbg[1] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
    // GREEN
    digitalWrite(s2_pin, HIGH);
    rbg[2] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  
  	return *rbg

}*/
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // HELPER FUNCTIONNNNNNNNNNNNNN for ULTRASONIC
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*void printUltra(float distance[], int trig[], int echo[], int polling_rate){
  	
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
}*/
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float* all_ultra(int trigPins[3], int echoPins[3])  //This probably should be within the the ultrasonic file, but I am not sure
{
  //go through all sensors from left right
  static float distances[3];
  for (int i; i < 3; i++) {
    distances[i] = ultra(trigPins[i], echoPins[i]);
  }
  return distances;
}

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


// --------------------------------------------------


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
    Serial.print(" --- G: ");
    Serial.print(green);
    Serial.print(" ---B: ");
    Serial.print(blue);
    Serial.print("----> ");
    }

    // determine color:
    int purple = (red + blue) / 2;
    if (red > threshold && green > threshold && blue > threshold) {
        return 1; // white
    } else if (red < black_threshold && blue < black_threshold && green < black_threshold) {
        return 0; // black
    } else if (red > 180 && green > threshold) {
        return 4; // green
    } else if (red > threshold && green > threshold) {
        return 6; // yellow
    }
    // else if (green > threshold){
    //     return 4;
    // }
    else if (red > threshold) {
        return 2; // red
    } else if (blue > 110) {
        return 3; // blue
    } else {
        return 5; // purple
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONNNNNNNNNNNNNN for MOTOR
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Some note:
	- need the self-adjust forward move using ultra?
    - WORSE SCENARIO - turn and hit the wall :(
*/
// #########################################################################

// Use to limit the direction range from 0 to 360
int validateDir(int dir) {
  if (dir > 360) return dir - 360;
  else if (dir < 0) return dir + 360;
  else return dir;
}

void rotateLeft(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed) {

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
}

void rotateRight(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed) {

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
}





void moveBackward(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed) {
  //setting the direction of both motors as backward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  //setting the speed of both motors
  analogWrite(ena, speed);  //  speed has to be between 0 and 255
  analogWrite(enb, speed);
}

void rotate180(int inA_1, int inA_2, int enA, int inB_1, int inB_2, int enB, int speed) {
  rotateRight(in1, in2, ena, in3, in4, enb, 255);
  rotateRight(in1, in2, ena, in3, in4, enb, 255);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONNNNNNNNNNNNNN for BLUETOOTH
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void messagePolling() {
  char c = ' ';

  /* // Keep reading from HM-10 and send to Arduino Serial Monitor
    if (BTSerial.available())
    Serial.write(BTSerial.read());

    // Keep reading from Arduino Serial Monitor and send to HM-10
    if (Serial.available()) {

        // Read from the Serial buffer (from the user input)
        c = Serial.read();
        // Do not send newline ('\n') nor carriage return ('\r') characters
        if(c != 10 && c != 13)
        	BTSerial.write(c);
        // If a newline ('\n') is true; print newline + prompt symbol; toggle
        if (new_line) { 
            Serial.print("\r\n>");
            new_line = false;
        }
        // Write to the Serial Monitor the bluetooth's response
        Serial.write(c);
      
        // If a newline ('\n') is read, toggle
        if (c == 10)
        	new_line = true;
  	}*/
}

/// THIS PART IS TO SEND THE MESSAGE
//
// 	Note: might need delay <----------
// ---------------------------------------------------------------------
void messageSend(String input_mess) {

  /*// Send a string for the bluetooth module to transmit 
  	BTSerial.print(input_mess); 

    // Print the same string to the Serial Monitor for feedback 
    Serial.println(input_mess);
  	
  	
  	delay(GATE_DELAY);*/
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HELPER FUNCTIONNNNNNNNNNNNNN for LED blinking
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void blinkGoal(int LED_pin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_pin, HIGH);
    delay(1000);
    digitalWrite(LED_pin, LOW);
    delay(1000);
  }
}
