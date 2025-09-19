#include "gamepad.h"
#include "gamepad_utils.h"
#include "config.h"
#include "gamepad_pinout.h"
#include "gamepad_assignment.h"
#include <stdarg.h>

// ============================================================================
// Global Variables
// ============================================================================

// Joystick data structures
JoystickData leftJoystick;
JoystickData rightJoystick;


// Gamepad state
bool gamepadDisabled = false;
bool sprintActive = false;

void printGamepad(const char* msg){
  #if DEBUG_PRINT_GAMEPAD
  Serial.print("Gamepad: ");
  Serial.println(msg);
  #endif
}

void printGamepad(const String& msg){
  #if DEBUG_PRINT_GAMEPAD
  Serial.print("Gamepad: ");
  Serial.println(msg);
  #endif
}

void printGamepadF(const char* format, ...){
  #if DEBUG_PRINT_GAMEPAD
  Serial.print("Gamepad: ");
  
  // Create a buffer for the formatted string
  char buffer[128];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  
  Serial.println(buffer);
  #endif
}

void setupGamepad()
{
  pinMode(PIN_GAMEPAD_ENABLE, INPUT_PULLUP);
  
  // Initialize joysticks
  initializeJoystick(leftJoystick, PIN_JOYSTICK_L_X, PIN_JOYSTICK_L_Y, PIN_JOYSTICK_L_SEL);
  initializeJoystick(rightJoystick, PIN_JOYSTICK_R_X, PIN_JOYSTICK_R_Y, PIN_JOYSTICK_R_SEL);

  delay(1000); // Wait a second to allow the joysticks to stabilize
  
  // Calibrate joysticks
  calibrateJoystick(leftJoystick);
  calibrateJoystick(rightJoystick);

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

    // Read and process joystick values
    readJoystick(leftJoystick, JOYSTICK_L_INVERT_X, JOYSTICK_L_INVERT_Y);
    readJoystick(rightJoystick, JOYSTICK_R_INVERT_X, JOYSTICK_R_INVERT_Y);
    
    // Process axis movements for directional keys
    processAxisMovement(leftJoystick, JOYSTICK_BINARY_THRESHOLD);
    
    // Handle mouse movement (right joystick)
    processMouseMovement(rightJoystick, JOYSTICK_MOUSE_SENSITIVITY);
    
    // Handle joystick button presses
    handleButtonPress(PIN_JOYSTICK_R_SEL, rightJoystick.selFlag, MOUSE_LEFT, "right joystick button");
    handleButtonPress(PIN_JOYSTICK_L_SEL, leftJoystick.selFlag, ACTION_JOYSTICK_L_PRESS, "left joystick button");
    
    // Handle directional keys (left joystick)
    handleDirectionalKeys(leftJoystick, ACTION_JOYSTICK_L_UP, ACTION_JOYSTICK_L_DOWN, 
                         ACTION_JOYSTICK_L_LEFT, ACTION_JOYSTICK_L_RIGHT, JOYSTICK_BINARY_THRESHOLD);
    
    // Handle sprint key
    if (SPRINT_THRESHOLD_ENABLED) {
      handleSprintKey(leftJoystick, ACTION_JOYSTICK_L_MAX, SPRINT_THRESHOLD, sprintActive);
    }

    // Debug output
    #ifdef DEBUG_PRINT_GAMEPAD
    static unsigned long lastPrint = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastPrint >= 500) {
      lastPrint = currentMillis;
      char buf[DEBUG_BUFFER_SIZE];
      snprintf(buf, sizeof(buf),
           "R Joy Y:%6d | R Joy X:%6d | L Joy Y:%6d | L Joy X:%6d",
           rightJoystick.yValue, rightJoystick.xValue, leftJoystick.yValue, leftJoystick.xValue);
      Serial.println(buf);
    }
    #endif

  } else {
    if (!gamepadDisabled){
      // Release all keys and mouse buttons when gamepad is disabled
      releaseAllMouseButtons();
      releaseAllKeys();
      
      // Reset joystick states
      leftJoystick.yPosPressed = false;
      leftJoystick.yNegPressed = false;
      leftJoystick.xPosPressed = false;
      leftJoystick.xNegPressed = false;
      leftJoystick.selFlag = 0;
      rightJoystick.selFlag = 0;
      sprintActive = false;
      
      gamepadDisabled = true;
      Serial.println("Gamepad disabled");
    }
    delay(1);
  }
}
