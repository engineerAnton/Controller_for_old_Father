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

int leftX = 639;
int deadLX = 530;
int rightX = 385;
int deadRX = 495;
int upY = 388;
int deadUpY = 485;
int deadDwnY = 535;
int dwnY = 632;

bool OkButtFlag = false;

long mPrevTime = 0;
long wPrevTime = 0;
long mDelay = 30;
long wDelay = 120;

Debouncer OkButton(OkButt, debounce_duration_ms);
Debouncer BackButton(BackButt, debounce_duration_ms);
Debouncer SearchButton(SearchButt, debounce_duration_ms);
Debouncer HomeButton(HomeButt, debounce_duration_ms);
Debouncer HystButton(HystButt, debounce_duration_ms);

void setup() {
  Serial.begin(9600);
  pinMode(debug, INPUT_PULLUP);
  pinMode(OkButt, INPUT_PULLUP);
  pinMode(BackButt, INPUT_PULLUP);
  pinMode(SearchButt, INPUT_PULLUP);
  pinMode(HomeButt, INPUT_PULLUP);
  pinMode(HystButt, INPUT_PULLUP);

  Mouse.begin();
  Keyboard.begin();
}

int readAxis(int axis) {
  
  // axis X
  if (axis == A1){
    if (analogRead(axis) >= deadLX){
      return -map(constrain(analogRead(axis), deadLX, leftX), deadLX, leftX, 0, range);
    }else if (analogRead(axis) <= deadRX){
      return map(constrain(analogRead(axis), rightX, deadRX), deadRX, rightX, 0, range);
    }else{
      return 0;
    }
  }

  // axis Y
  if (axis == A0){
    if (analogRead(axis) >= deadDwnY){
      return map(constrain(analogRead(axis), deadDwnY, dwnY), deadDwnY, dwnY, 0, range);
    }else if (analogRead(axis) <= deadUpY){
      return -map(constrain(analogRead(axis), upY, deadUpY), deadUpY, upY, 0, range);
    }else{
      return 0;
    }
  }
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
  if (digitalRead(SearchButt) == LOW && digitalRead(HystButt) == LOW){

    //Debug analog
    Serial.print("Xa: ");
    Serial.print(analogRead(A1));
    Serial.print(" Ya: ");
    Serial.println(analogRead(A0));
    
  } else if (digitalRead(HomeButt) == LOW && digitalRead(HystButt) == LOW){
  
  //Debug
    Serial.print("X: ");
    Serial.print(readAxis(xAxis));
    Serial.print(" Y: ");
    Serial.println(readAxis(yAxis));
  
  }else{

    //Main code
    OkButton.update();
    BackButton.update();
    SearchButton.update();
    HomeButton.update();
    HystButton.update();
    
    // read and scale the two axes:
    int xReading = readAxis(xAxis);
    int yReading = readAxis(yAxis);
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
      Mouse.release();
    }

    if (OkButton.falling()){
      Mouse.press();
    }

    if (BackButton.falling()){
      Mouse.click(MOUSE_RIGHT);

    }else if(SearchButton.falling()){
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('f');
      delay(200);
      Keyboard.releaseAll();

    }else if(HomeButton.falling()){
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_ESC);
      delay(200);
      Keyboard.releaseAll();

    }else if(HystButton.falling()){
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('h');
      delay(200);
      Keyboard.releaseAll();
    }
  }
}
