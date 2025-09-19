#include "gamepad_utils.h"
#include "config.h"
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

void readJoystick(JoystickData& joystick, int invertX, int invertY) {
    joystick.yValue = (analogRead(joystick.yPin) - joystick.yZero) * invertY;
    joystick.xValue = (analogRead(joystick.xPin) - joystick.xZero) * invertX;
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
    // Skip processing if action is ACTION_NONE
    if (key == ACTION_NONE) {
        return;
    }
    
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
        if (upKey != ACTION_NONE) Keyboard.press(upKey);
        if (downKey != ACTION_NONE) Keyboard.release(downKey);
    } else if (joystick.yNegPressed) {
        if (downKey != ACTION_NONE) Keyboard.press(downKey);
        if (upKey != ACTION_NONE) Keyboard.release(upKey);
    } else {
        if (upKey != ACTION_NONE) Keyboard.release(upKey);
        if (downKey != ACTION_NONE) Keyboard.release(downKey);
    }
    
    // Handle horizontal movement
    if (joystick.xPosPressed) {
        if (leftKey != ACTION_NONE) Keyboard.press(leftKey);
        if (rightKey != ACTION_NONE) Keyboard.release(rightKey);
    } else if (joystick.xNegPressed) {
        if (rightKey != ACTION_NONE) Keyboard.press(rightKey);
        if (leftKey != ACTION_NONE) Keyboard.release(leftKey);
    } else {
        if (leftKey != ACTION_NONE) Keyboard.release(leftKey);
        if (rightKey != ACTION_NONE) Keyboard.release(rightKey);
    }
}

void handleSprintKey(JoystickData& joystick, uint8_t sprintKey, int threshold, bool& active) {
    // Skip processing if action is ACTION_NONE
    if (sprintKey == ACTION_NONE) {
        return;
    }
    
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

void processMouseMovement(JoystickData& joystick, int sensitivity) {
    // Apply deadzone to prevent drift
    int xValue = abs(joystick.xValue) > JOYSTICK_X_DEADZONE ? joystick.xValue : 0;
    int yValue = abs(joystick.yValue) > JOYSTICK_Y_DEADZONE ? joystick.yValue : 0;
    
    #if DEBUG_PRINT_GAMEPAD
    static unsigned long lastDebug = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebug >= 200) { // Debug every 200ms
        lastDebug = currentMillis;
        Serial.print("Mouse Debug - Raw X: ");
        Serial.print(joystick.xValue);
        Serial.print(", Raw Y: ");
        Serial.print(joystick.yValue);
        Serial.print(", Filtered X: ");
        Serial.print(xValue);
        Serial.print(", Filtered Y: ");
        Serial.println(yValue);
    }
    #endif
    
    // If no movement in either axis, don't move the mouse
    if (xValue == 0 && yValue == 0) {
        return;
    }
    
    // Multi-zone scaling for better precision and range
    float xDelta = calculateMouseDelta(xValue, sensitivity);
    float yDelta = calculateMouseDelta(yValue, sensitivity);
    
    #if DEBUG_PRINT_GAMEPAD
    if (currentMillis - lastDebug < 50) { // Only print if we just printed above
        Serial.print("Deltas - X: ");
        Serial.print(xDelta);
        Serial.print(", Y: ");
        Serial.println(yDelta);
    }
    #endif
    
    // Apply magnitude-based scaling to maintain consistent speed in all directions
    // This ensures diagonal movement feels the same speed as cardinal directions
    float magnitude = sqrt(xDelta * xDelta + yDelta * yDelta);
    if (magnitude > 0) {
        // Normalize to prevent faster diagonal movement
        float maxMagnitude = 10.0f; // Maximum mouse movement per frame
        if (magnitude > maxMagnitude) {
            xDelta = (xDelta / magnitude) * maxMagnitude;
            yDelta = (yDelta / magnitude) * maxMagnitude;
        }
    }
    
    // Move mouse with both X and Y deltas in a single call for smooth diagonal movement
    int moveX = (int)xDelta;
    int moveY = (int)yDelta;
    
    #if DEBUG_PRINT_GAMEPAD
    if (currentMillis - lastDebug < 50) { // Only print if we just printed above
        Serial.print("Final Move - X: ");
        Serial.print(moveX);
        Serial.print(", Y: ");
        Serial.println(moveY);
    }
    #endif
    
    if (moveX != 0 || moveY != 0) {
        Mouse.move(moveX, moveY);
    }
}

// ============================================================================
// Advanced Mouse Scaling Functions
// ============================================================================

float calculateMouseDelta(int joystickValue, int sensitivity) {
    // Get absolute value for calculations
    int absValue = abs(joystickValue);
    int sign = joystickValue >= 0 ? 1 : -1;
    
    // Multi-zone scaling for different precision levels
    float delta = 0.0f;
    
    if (absValue <= 100) {
        // Precision zone (0-100): Very sensitive for fine movements
        // Linear scaling with high sensitivity
        delta = (float)absValue / 20.0f; // 0-5 pixel range
    }
    else if (absValue <= 300) {
        // Normal zone (100-300): Balanced sensitivity
        // Linear scaling with medium sensitivity
        delta = 5.0f + ((float)(absValue - 100) / 25.0f); // 5-13 pixel range
    }
    else {
        // Fast zone (300-500): Reduced sensitivity for quick movements
        // Exponential curve for natural acceleration
        float normalizedValue = (float)(absValue - 300) / 200.0f; // 0.0 to 1.0
        delta = 13.0f + (normalizedValue * normalizedValue * 7.0f); // 13-20 pixel range
    }
    
    // Apply global sensitivity scaling
    delta = delta * (100.0f / sensitivity);
    
    // Ensure minimum movement for very small inputs
    if (delta < 0.1f && absValue > 0) {
        delta = 0.1f;
    }
    
    return delta * sign;
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
