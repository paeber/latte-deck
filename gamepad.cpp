#include "gamepad.h"
#include "config.h"


// Right
int vertZeroR, horzZeroR;  // Stores the initial value of each axis, usually around 512
int vertValueR, horzValueR;  // Stores current analog output of each axis
const int sensitivityR = 160;  // Higher sensitivity value = slower mouse, should be <= about 500
int mouseClickFlagR = 0;
//int invertMouse = 1;        //Invert joystick based on orientation
int invertMouseR = -1;         //Noninverted joystick based on orientation
int invertJoystickLX = -1;
int invertJoystickLY = -1;


// Left
int vertZeroL, horzZeroL;  // Stores the initial value of each axis, usually around 512
int vertValueL, horzValueL;  // Stores current analog output of each axis
bool vertPosPressedL = false;
bool vertNegPressedL = false;
bool horzPosPressedL = false;
bool horzNegPressedL = false;
bool sprintActive = false;
int selFlagL = 0;

bool gamepadDisabled = false;

template <typename T> int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

void setupGamepad()
{
  pinMode(PIN_GAMEPAD_ENABLE, INPUT_PULLUP);
  //digitalWrite(PIN_GAMEPAD_ENABLE, HIGH);
  
  // Right side
  pinMode(PIN_JOYSTICK_R_X, INPUT);  
  pinMode(PIN_JOYSTICK_R_Y, INPUT);
  pinMode(PIN_JOYSTICK_R_SEL, INPUT_PULLUP);

  // Left side
  pinMode(PIN_JOYSTICK_L_X, INPUT);
  pinMode(PIN_JOYSTICK_L_Y, INPUT);
  pinMode(PIN_JOYSTICK_L_SEL, INPUT_PULLUP);

  delay(1000); // Wait a second to allow the joysticks to stabilize
  // Calibrate joysticks
  vertZeroR = analogRead(PIN_JOYSTICK_R_Y);  
  horzZeroR = analogRead(PIN_JOYSTICK_R_X);

  vertZeroL = analogRead(PIN_JOYSTICK_L_Y);
  horzZeroL = analogRead(PIN_JOYSTICK_L_X);

  // CompositeHID::begin() is called from main setup
  Serial.println("Gamepad ready");
}

void loopGamepad()
{
  if (!digitalRead(PIN_GAMEPAD_ENABLE)) 
  {
    if(gamepadDisabled){
      Serial.println("Gamepad enabled");
    }
    gamepadDisabled = false;

    // Read joystick values
    vertValueR = analogRead(PIN_JOYSTICK_R_Y) - vertZeroR;  // read vertical offset
    horzValueR = -(analogRead(PIN_JOYSTICK_R_X) - horzZeroR);  // read horizontal offset
    vertValueL = analogRead(PIN_JOYSTICK_L_Y) - vertZeroL;  // read vertical offset
    horzValueL = -(analogRead(PIN_JOYSTICK_L_X) - horzZeroL);  // read horizontal offset

    // Clip values to max if exceeding
    if (abs(horzValueR) > JOYSTICK_SIDE_MAX){
      horzValueR = sgn(horzValueR) * JOYSTICK_SIDE_MAX;
    }
    if (abs(vertValueR) > JOYSTICK_SIDE_MAX){
      vertValueR = sgn(vertValueR) * JOYSTICK_SIDE_MAX;
    }

    if (abs(horzValueL) > JOYSTICK_SIDE_MAX){
      horzValueL = sgn(horzValueL) * JOYSTICK_SIDE_MAX;
    }
    if (abs(vertValueL) > JOYSTICK_SIDE_MAX){
      vertValueL = sgn(vertValueL) * JOYSTICK_SIDE_MAX;
    }
  
    if (vertValueR != 0)
      CompositeHID::moveMouse(0, (invertMouseR * sgn(vertValueR) * 0.1 * (abs(pow(vertValueR /sensitivityR, 4))))); // move mouse on y axis
    if (horzValueR != 0)
      CompositeHID::moveMouse((invertMouseR * sgn(horzValueR) * 0.1 * (abs(pow(horzValueR / sensitivityR, 4)))), 0); // move mouse on x axis
  
    if ((digitalRead(PIN_JOYSTICK_R_SEL) == 0) && (!mouseClickFlagR))  // if the joystick button is pressed
    {
      mouseClickFlagR = 1;
      CompositeHID::clickMouse(0x01);  // click the left button down
    }
    else if ((digitalRead(PIN_JOYSTICK_R_SEL)) && (mouseClickFlagR)) // if the joystick button is not pressed
    {
      mouseClickFlagR = 0;
      CompositeHID::releaseMouse(0x01);  // release the left button
    }

   
  
    if ((digitalRead(PIN_JOYSTICK_L_SEL) == 0) && (!selFlagL))  // if the joystick button is pressed
    {
      selFlagL = 1;
      CompositeHID::pressKey(ACTION_JOYSTICK_L_PRESS);  // click the left button down
    }
    else if ((digitalRead(PIN_JOYSTICK_L_SEL)) && (selFlagL)) // if the joystick button is not pressed
    {
      selFlagL = 0;
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_PRESS);  // release the left button
    }

    if ((vertValueL >= 200) && (!vertPosPressedL)){
      CompositeHID::pressKey(ACTION_JOYSTICK_L_UP);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_DOWN);
      vertPosPressedL = true;
      vertNegPressedL = false;
    } else if ((vertValueL <= -200) && (!vertNegPressedL)) {
      CompositeHID::pressKey(ACTION_JOYSTICK_L_DOWN);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_UP);
      vertNegPressedL = true;
      vertPosPressedL = false;
    } else if ((vertPosPressedL && vertValueL < 200) || (vertNegPressedL && vertValueL > (-200))) {
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_UP);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_DOWN);
      vertNegPressedL = false;
      vertPosPressedL = false;
    }

    if ((horzValueL >= 200) && (!horzPosPressedL)){
      CompositeHID::pressKey(ACTION_JOYSTICK_L_LEFT);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_RIGHT);
      horzPosPressedL = true;
      horzNegPressedL = false;
    } else if ((horzValueL <= -200) && (!horzNegPressedL)) {
      CompositeHID::pressKey(ACTION_JOYSTICK_L_RIGHT);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_LEFT);
      horzNegPressedL = true;
      horzPosPressedL = false;
    } else if ((horzPosPressedL && horzValueL < 200) || (horzNegPressedL && horzValueL > (-200))) {
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_LEFT);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_RIGHT);
      horzNegPressedL = false;
      horzPosPressedL = false;
    }

    if (((abs(horzValueL) >= SPRINT_THRESHOLD) || (abs(vertValueL) >= SPRINT_THRESHOLD)) && (!sprintActive)) {
      CompositeHID::pressKey(ACTION_JOYSTICK_L_MAX);
      sprintActive = true;
    } else if (((abs(horzValueL) < (SPRINT_THRESHOLD -20)) && (abs(vertValueL) < (SPRINT_THRESHOLD -20))) && (sprintActive)) {
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_MAX);
      sprintActive = false;
    }


    #ifdef DEBUG_PRINT_GAMEPAD
    static unsigned long lastPrint = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastPrint >= 500) {
      lastPrint = currentMillis;
      char buf[DEBUG_BUFFER_SIZE]; // Configurable buffer size
      snprintf(buf, sizeof(buf),
           "R Joy Y:%6d | R Joy X:%6d | L Joy Y:%6d | L Joy X:%6d",
           vertValueR, horzValueR, vertValueL, horzValueL);
      Serial.println(buf);
    }
    #endif

    
    } else {
    if (!gamepadDisabled){
      CompositeHID::releaseMouse(0x01);
      CompositeHID::releaseMouse(0x02);
      CompositeHID::moveMouse(0, 0);
      mouseClickFlagR = 0;

      CompositeHID::releaseKey(ACTION_JOYSTICK_L_UP);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_DOWN);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_MAX);
      CompositeHID::releaseKey(ACTION_JOYSTICK_L_PRESS);
      CompositeHID::releaseKey(ACTION_BTN_L2);
      CompositeHID::releaseKey(ACTION_BTN_L3);
      CompositeHID::releaseKey(ACTION_BTN_L4);

      CompositeHID::releaseKey(ACTION_BTN_R2);
      CompositeHID::releaseKey(ACTION_BTN_R3);
      CompositeHID::releaseKey(ACTION_BTN_R4);

      vertNegPressedL = false;
      vertPosPressedL = false;
      selFlagL = 0;
      gamepadDisabled = true;
      Serial.println("Gamepad disabled");
    }
    delay(1);
  }
 
}
