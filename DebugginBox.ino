#include <Keyboard.h>
int Led = 13 ; // Arduino built in LED
int val ; // define numeric variables
int baseVal = 0;
int calibrate = 5;
int check = 0;

#define OFF_ON 9
#define BACKTRACK 10
#define NEXT_STEP 11
#define INFO_LOCALS 15
#define BP_MAIN 16
#define SENSOR A0

void readAndDisplay();
void performAction();

class Button {
  public:
    void writeCommand() {
      Keyboard.println(message);
    }
    virtual int updateState(int) = 0;
    String message;
};

class PressButton: public Button {
  public:
    PressButton(String s) {
      message = s;
    }
    bool wasToggled() {
      bool crt = toggled;
      if (toggled) {
        toggled = false;
      }
      return crt;
    }
    int updateState(int state) {
      if (state) {
        toggled = true;
        return true;
      }
      return false;
    }
  private:
    bool toggled;
};

class SwitchButton: public Button {
  public:
    SwitchButton(String on, String off) {
      message = on;
      messageOff = off;
    }
    void writeCommand() {
      if (state) {
        Keyboard.println(message);
      } else {
        Keyboard.println(messageOff);
      }
    }
    int updateState(int newVal) {
      if (bool(newVal) == state)
        return false;
      if (newVal) {
        state = true;
      } else {
        state = false;
      }
      return true;
    }
    int getState() {
      return state;
    }
  private:
    String messageOff;
    bool state;
};


SwitchButton off_on ("kill\ny", "run\n");
SwitchButton bt_regular ("bt", "regular");
SwitchButton next_step ("next", "step");
PressButton bp_main("break main");
PressButton info_locals("info locals");

void setup ()
{
  Serial.begin(9600);
  pinMode (SENSOR, INPUT) ;  // define the Hall magnetic sensor line as input
  pinMode (OFF_ON, INPUT);
  pinMode (BACKTRACK, INPUT);
  pinMode (BP_MAIN, INPUT);
  pinMode (INFO_LOCALS, INPUT);

  pinMode (NEXT_STEP, INPUT);

  for (int i = 0; i < calibrate; i++) {
    baseVal += analogRead (SENSOR);
    delay(5);
  }
  baseVal /= calibrate;
  Serial.println(baseVal);
  Keyboard.begin();
}

void loop ()
{
  val = analogRead (SENSOR) ; // read sensor line
  readAndDisplay();
  if (abs(val - baseVal) > 20) {
    if (check == 0) {
//      Serial.println("rotated");
      check = 1;
      delay(200);
      performAction();
    }
  } else if (check == 1) {
    check = 0;
  }
  if (bp_main.wasToggled()) {
    bp_main.writeCommand();
    delay(200);
  }
  if (info_locals.wasToggled()) {
    info_locals.writeCommand();
    delay(200);
  }
  delay(100);
}

void readAndDisplay()
{
  int x = digitalRead(OFF_ON);
  int y = digitalRead(BACKTRACK);
  int z = digitalRead(NEXT_STEP);
  int aa = digitalRead(BP_MAIN);
  int bb = digitalRead(INFO_LOCALS);
  //  Serial.println("OFF_ON, BACKTRACK, NEXT_STEP, BP_MAIN, INFO_LOCALS");
  //  Serial.print(x);
  //  Serial.print(y);
  //  Serial.print(z);
  //  Serial.print(aa);
  //  Serial.println(bb);
  if (off_on.updateState(x)) {
    off_on.writeCommand();
  }
  bt_regular.updateState(y);
  next_step.updateState(z);
  bp_main.updateState(aa);
  info_locals.updateState(bb);
}

void performAction()
{
  if (!off_on.getState()) { // it's on
    if (bt_regular.getState()) {     // backtrack state
      Keyboard.println("bt full");
      Keyboard.println("info threads");
    } else { // regular state
      next_step.writeCommand();

    }
  }
}
