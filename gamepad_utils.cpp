#include "gamepad_utils.h"
#include <HID-Project.h>
#include <math.h>

// ============================================================================
// Joystick Management Functions
// ============================================================================

void initializeJoystick(JoystickData& joystick, int xPin, int yPin, int selPin) {
    joystick.xPin = xPin;
    joystick.yPin = yPin;
    joystick.selPin = selPin;
    joystick.xZero = 0;
    joystick.yZero = 0;
    joystick.xValue = 0;
    joystick.yValue = 0;
    joystick.magnitude = 0;
    joystick.selFlag = 0;
    joystick.xPosPressed = false;
    joystick.xNegPressed = false;
    joystick.yPosPressed = false;
    joystick.yNegPressed = false;
    
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(selPin, INPUT_PULLUP);
}

void calibrateJoystick(JoystickData& joystick) {
    joystick.xZero = analogRead(joystick.xPin);
    joystick.yZero = analogRead(joystick.yPin);
}

void readJoystick(JoystickData& joystick) {
    joystick.yValue = analogRead(joystick.yPin) - joystick.yZero;
    joystick.xValue = -(analogRead(joystick.xPin) - joystick.xZero);
    joystick.magnitude = calculateMagnitude(joystick.xValue, joystick.yValue);
    
    // Clip values to maximum
    joystick.xValue = clipAxisValue(joystick.xValue, JOYSTICK_SIDE_MAX);
    joystick.yValue = clipAxisValue(joystick.yValue, JOYSTICK_SIDE_MAX);
}

// ============================================================================
// Axis Processing Functions
// ============================================================================

int clipAxisValue(int value, int maxValue) {
    if (abs(value) > maxValue) {
        return sgn(value) * maxValue;
    }
    return value;
}

void processAxisMovement(JoystickData& joystick, int threshold) {
    // Process Y-axis (vertical) movement
    if ((joystick.yValue >= threshold) && (!joystick.yPosPressed)) {
        joystick.yPosPressed = true;
        joystick.yNegPressed = false;
    } else if ((joystick.yValue <= -threshold) && (!joystick.yNegPressed)) {
        joystick.yNegPressed = true;
        joystick.yPosPressed = false;
    } else if ((joystick.yPosPressed && joystick.yValue < threshold) || 
               (joystick.yNegPressed && joystick.yValue > -threshold)) {
        joystick.yNegPressed = false;
        joystick.yPosPressed = false;
    }
    
    // Process X-axis (horizontal) movement
    if ((joystick.xValue >= threshold) && (!joystick.xPosPressed)) {
        joystick.xPosPressed = true;
        joystick.xNegPressed = false;
    } else if ((joystick.xValue <= -threshold) && (!joystick.xNegPressed)) {
        joystick.xNegPressed = true;
        joystick.xPosPressed = false;
    } else if ((joystick.xPosPressed && joystick.xValue < threshold) || 
               (joystick.xNegPressed && joystick.xValue > -threshold)) {
        joystick.xNegPressed = false;
        joystick.xPosPressed = false;
    }
}

// ============================================================================
// Button Handling Functions
// ============================================================================

void handleButtonPress(int pin, int& flag, uint8_t key, const char* action) {
    if ((digitalRead(pin) == 0) && (!flag)) {
        flag = 1;
        Keyboard.press(key);
        #if DEBUG_PRINT_GAMEPAD
        Serial.print("Gamepad: Pressing ");
        Serial.println(action);
        #endif
    } else if ((digitalRead(pin)) && (flag)) {
        flag = 0;
        Keyboard.release(key);
        #if DEBUG_PRINT_GAMEPAD
        Serial.print("Gamepad: Releasing ");
        Serial.println(action);
        #endif
    }
}

void handleDirectionalKeys(JoystickData& joystick, uint8_t upKey, uint8_t downKey, 
                          uint8_t leftKey, uint8_t rightKey, int threshold) {
    // Handle vertical movement
    if (joystick.yPosPressed) {
        Keyboard.press(upKey);
        Keyboard.release(downKey);
    } else if (joystick.yNegPressed) {
        Keyboard.press(downKey);
        Keyboard.release(upKey);
    } else {
        Keyboard.release(upKey);
        Keyboard.release(downKey);
    }
    
    // Handle horizontal movement
    if (joystick.xPosPressed) {
        Keyboard.press(leftKey);
        Keyboard.release(rightKey);
    } else if (joystick.xNegPressed) {
        Keyboard.press(rightKey);
        Keyboard.release(leftKey);
    } else {
        Keyboard.release(leftKey);
        Keyboard.release(rightKey);
    }
}

void handleSprintKey(JoystickData& joystick, uint8_t sprintKey, int threshold, bool& active) {
    if ((abs(joystick.magnitude) >= threshold) && (!active)) {
        Keyboard.press(sprintKey);
        active = true;
        #if DEBUG_PRINT_GAMEPAD
        Serial.println("Gamepad: Pressing sprint");
        #endif
    } else if ((abs(joystick.magnitude) < (threshold - 20)) && (active)) {
        Keyboard.release(sprintKey);
        active = false;
        #if DEBUG_PRINT_GAMEPAD
        Serial.println("Gamepad: Releasing sprint");
        #endif
    }
}

// ============================================================================
// Mouse Control Functions
// ============================================================================

void processMouseMovement(JoystickData& joystick, int sensitivity, int invert) {
    if (joystick.yValue != 0) {
        Mouse.move(0, (invert * sgn(joystick.yValue) * 0.01 * (abs(pow(joystick.yValue, 2)) / sensitivity)));
    }
    if (joystick.xValue != 0) {
        Mouse.move((invert * sgn(joystick.xValue) * 0.01 * (abs(pow(joystick.xValue, 2)) / sensitivity)), 0);
    }
}

// ============================================================================
// Key Release Management Functions
// ============================================================================

void releaseAllKeys() {
    Keyboard.release(ACTION_JOYSTICK_L_UP);
    Keyboard.release(ACTION_JOYSTICK_L_DOWN);
    Keyboard.release(ACTION_JOYSTICK_L_MAX);
    Keyboard.release(ACTION_JOYSTICK_L_PRESS);
    Keyboard.release(ACTION_BTN_L2);
    Keyboard.release(ACTION_BTN_L3);
    Keyboard.release(ACTION_BTN_L4);
    Keyboard.release(ACTION_BTN_R2);
    Keyboard.release(ACTION_BTN_R3);
    Keyboard.release(ACTION_BTN_R4);
}

void releaseAllMouseButtons() {
    Mouse.release(MOUSE_LEFT);
    Mouse.release(MOUSE_RIGHT);
    Mouse.move(0, 0);
}

// ============================================================================
// Utility Functions
// ============================================================================

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float calculateMagnitude(int x, int y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}
