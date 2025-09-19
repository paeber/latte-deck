#ifndef GAMEPAD_UTILS_H
#define GAMEPAD_UTILS_H

#include <Arduino.h>
#include "config.h"
#include "gamepad_pinout.h"
#include "gamepad_assignment.h"

// ============================================================================
// Joystick Data Structure
// ============================================================================

struct JoystickData {
    int xPin, yPin, selPin;
    int xZero, yZero;
    int xValue, yValue;
    float magnitude;
    int selFlag;
    bool xPosPressed, xNegPressed;
    bool yPosPressed, yNegPressed;
};

// ============================================================================
// Function Prototypes
// ============================================================================

// Joystick Management
void initializeJoystick(JoystickData& joystick, int xPin, int yPin, int selPin);
void readJoystick(JoystickData& joystick, int invertX, int invertY);
void calibrateJoystick(JoystickData& joystick);

// Axis Processing
int clipAxisValue(int value, int maxValue);
void processAxisMovement(JoystickData& joystick, int threshold = 200);

// Button Handling
void handleButtonPress(int pin, int& flag, uint8_t key, const char* action);
void handleDirectionalKeys(JoystickData& joystick, uint8_t upKey, uint8_t downKey, 
                          uint8_t leftKey, uint8_t rightKey, int threshold = 200);
void handleSprintKey(JoystickData& joystick, uint8_t sprintKey, int threshold, bool& active);

// Mouse Control
void processMouseMovement(JoystickData& joystick, int sensitivity);
float calculateMouseDelta(int joystickValue, int sensitivity);

// Key Release Management
void releaseAllKeys();
void releaseAllMouseButtons();

// Utility Functions
template <typename T> int sgn(T val);
float calculateMagnitude(int x, int y);

#endif // GAMEPAD_UTILS_H
