#include <Arduino.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <Debouncer.h>

int debounce_duration_ms = 50;

const int xAxis = A1;         //analog sensor for X axis
const int yAxis = A0;         // analog sensor for Y axis
const int wAxis = A2;
const byte debug = 15;
const byte OkButt = 4;
const byte BackButt = 6;
const byte SearchButt = 2;
const byte HomeButt = 3;
const byte HystButt = 5;

int range = 60;               // output range of X or Y movement 12
int responseDelay = 60;        // response delay of the mouse, in ms
int threshold = range / 8;    // resting threshold
int center = range / 2;       // resting position value
int minima[] = {386, 383};  // actual analogRead minima for {x, y}
int maxima[] = {640, 635};        // actual analogRead maxima for {x, y}
int axis[] = {xAxis, yAxis};  // pin numbers for {x, y}
int mouseReading[2];          // final mouse readings for {x, y}

long mPrevTime = 0;
long wPrevTime = 0;
long mDelay = 20;
long wDelay = 120;
bool flag = 0;

Debouncer OkButton(OkButt, debounce_duration_ms);
Debouncer BackButton(BackButt, debounce_duration_ms);
Debouncer SearchButton(SearchButt, debounce_duration_ms);
Debouncer HomeButton(HomeButt, debounce_duration_ms);
Debouncer HystButton(HystButt, debounce_duration_ms);

void setup() {
  //Serial.begin(9600);
  pinMode(debug, INPUT_PULLUP);
  pinMode(OkButt, INPUT_PULLUP);
  pinMode(BackButt, INPUT_PULLUP);
  pinMode(SearchButt, INPUT_PULLUP);
  pinMode(HomeButt, INPUT_PULLUP);
  pinMode(HystButt, INPUT_PULLUP);

  Mouse.begin();
  Keyboard.begin();
}

int readAxis(int axisNumber) {
  int distance = 0; // distance from center of the output range

  // read the analog input:
  int reading = analogRead(axis[axisNumber]);

  // of the current reading exceeds the max or min for this axis,
  // reset the max or min:
  if (reading < minima[axisNumber]) {
    minima[axisNumber] = reading;
  }
  if (reading > maxima[axisNumber]) {
    maxima[axisNumber] = reading;
  }

  // map the reading from the analog input range to the output range:
  reading = map(reading, minima[axisNumber], maxima[axisNumber], 0, range);

  // if the output reading is outside from the
  // rest position threshold,  use it:
  if (abs(reading - center) > threshold) {
    distance = (reading - center);
  }

  // the Y axis needs to be inverted in order to
  // map the movemment correctly:
  if (axisNumber == 0) {
    distance = -distance;
  }

  // return the distance for this axis:
  return distance;
}

int readWheel(int axis){
  int distance; // distance from center of the output range
  
  // read the analog input:
  int reading = analogRead(axis);

  if (reading > 595 && reading < 655) distance = -1;
  else if (reading > 655) distance = -2;
  else if (reading < 420 && reading > 355) distance = 1;
  else if (reading < 355) distance = 2;
  else distance = 0;

  // return the distance for this axis:
  return distance;  
}

void loop() {
  if (digitalRead(debug) == HIGH){

    OkButton.update();
    BackButton.update();
    SearchButton.update();
    HomeButton.update();
    HystButton.update();
    
    // read and scale the two axes:
    int xReading = readAxis(0);
    int yReading = readAxis(1);
    int wReading = readWheel(wAxis);

    long mCurrTime = millis();
    long wCurrTime = millis();

    if (mCurrTime - mPrevTime > mDelay){
      mPrevTime = mCurrTime;
      Mouse.move(xReading, yReading, 0);
    }
  
    if (wCurrTime - wPrevTime > wDelay){
      wPrevTime = wCurrTime;
      Mouse.move(0, 0, wReading);
    }
  
    // buttons
    if (OkButton.rising()){
      Mouse.click();
    }

    if (BackButton.rising()){
      Mouse.click(MOUSE_RIGHT);

    }else if(SearchButton.rising()){
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('f');
      delay(100);
      Keyboard.releaseAll();

    }else if(HomeButton.rising()){
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_ESC);
      delay(100);
      Keyboard.releaseAll();

    }else if(HystButton.rising()){
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('h');
      delay(100);
      Keyboard.releaseAll();
    }   
  
  }else{
    Serial.println("debug");
  }
}

