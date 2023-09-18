#define s0_color 14
#define s1_color 15
#define s2_color 16
#define s3_color 17
#define out_color 18

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


void setup() {
    pinMode(s0_color, OUTPUT);
    pinMode(s1_color, OUTPUT);
    pinMode(s2_color, OUTPUT);
    pinMode(s3_color, OUTPUT);
    pinMode(out_color, INPUT);
    digitalWrite(s0_color, HIGH);
    digitalWrite(s1_color, HIGH);
    Serial.begin(9600);

    calibrate();
}

//////////////////////////////////////////////////////////////////////////////////
// 0 black, 1 white, 2 red, 3 4 5 6 - blue green purple yellow
/////////////////////////////////////////////////////////////////////////////////

void loop() {
    int color = determineColor(1);
    // Serial.println(color);
    
    delay(200);
}

// RETURN THE INTEGER VALUE OF THE COLOR - Omar dictionary
//
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
    // int purple = (red + blue) / 2;
    // if (red > threshold && green > threshold && blue > threshold) {
    //     Serial.print("white");
    //     return 1; // white
    // } else if (red < black_threshold && blue < black_threshold && green < black_threshold) {
    //     Serial.print("black");
    //     return 0; // black
    // } else if (red > 180 && green > threshold) {
    //     Serial.print("green");
    //     return 4; // green
    // } else if (red > threshold && green > threshold) {
    //     Serial.print("yellow");
    //     return 6; // yellow
    // }
    // // else if (green > threshold){
    // //     return 4;
    // // }
    // else if (red > threshold) {
    //     Serial.print("red");
    //     return 2; // red
    // } else if (blue > 110) {
    //     Serial.print("blue");
    //     return 3; // blue
    // } else {
    //     Serial.print("purple");
    //     return 5; // purple
    // }
// }

////////////////////////////////////////////////////////////////////////
void calibrate() {
    Serial.println("Put black infront");
    while (Serial.read() != 'c') {} //wait

    // Get color ----------------------------------------------------------------
    digitalWrite(s2_color, LOW);
    digitalWrite(s3_color, LOW);
    int red = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s3_color, HIGH);
    int blue = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s2_color, HIGH);
    int green = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);

    //  COLOR --------------------------------------------------------
    
    Serial.print("R: ");
    Serial.print(red);
    Serial.print(" --- G: ");
    Serial.print(green);
    Serial.print(" ---B: ");
    Serial.print(blue);
    Serial.println("----> ");

    Serial.println("Put white infront");
    while (Serial.read() != 'c') {} //wait

    // Get color ----------------------------------------------------------------
    digitalWrite(s2_color, LOW);
    digitalWrite(s3_color, LOW);
    red = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s3_color, HIGH);
    blue = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    digitalWrite(s2_color, HIGH);
    green = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
    //  COLOR --------------------------------------------------------

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
////////////////////////////////////////////////////////////////////////////////

// int* color() {
//   // int tmp_arr[3] = {0,0,0};
//   static int* rbg[3] = { 0, 0, 0 };

//   // apply filter for measuring R, B, G values
//   digitalWrite(s2_color, LOW);
//   digitalWrite(s3_color, LOW);
//   // RED
//   rbg[0] = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
//   // BLUE
//   digitalWrite(s3_color, HIGH);
//   rbg[1] = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);
//   // GREEN
//   digitalWrite(s2_color, HIGH);
//   rbg[2] = pulseIn(out_color, digitalRead(out_color) == HIGH ? LOW : HIGH);

//   return rbg;
// }
