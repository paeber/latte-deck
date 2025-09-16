# Modular Architecture Documentation

## Overview

The LatteDeck project has been refactored to use a modular architecture that eliminates code duplication and improves maintainability. The code is now organized into focused, reusable functions that handle specific aspects of gamepad functionality.

## Architecture Components

### Core Files

- **`gamepad.cpp`** - Main gamepad logic and state management
- **`gamepad.h`** - Public interface declarations
- **`gamepad_utils.cpp`** - Utility functions for joystick and button handling
- **`gamepad_utils.h`** - Utility function declarations and data structures

### Data Structures

#### JoystickData Structure
```cpp
struct JoystickData {
    int xPin, yPin, selPin;        // Pin assignments
    int xZero, yZero;              // Calibration values
    int xValue, yValue;            // Current axis values
    float magnitude;               // Movement magnitude
    int selFlag;                   // Button state flag
    bool xPosPressed, xNegPressed; // X-axis press states
    bool yPosPressed, yNegPressed; // Y-axis press states
};
```

## Function Categories

### 1. Joystick Management Functions

#### `initializeJoystick(JoystickData& joystick, int xPin, int yPin, int selPin)`
- Initializes joystick data structure
- Sets up pin modes (INPUT for axes, INPUT_PULLUP for button)
- Resets all state variables

#### `calibrateJoystick(JoystickData& joystick)`
- Reads initial axis values for calibration
- Stores zero-point values for offset calculation

#### `readJoystick(JoystickData& joystick, int invertX, int invertY)`
- Reads current analog values from joystick pins
- Calculates offset from zero-point
- Applies axis inversion based on invertX and invertY parameters
- Applies axis clipping to prevent overflow
- Calculates movement magnitude

### 2. Axis Processing Functions

#### `clipAxisValue(int value, int maxValue)`
- Clips axis values to prevent exceeding maximum range
- Maintains sign of original value

#### `processAxisMovement(JoystickData& joystick, int threshold)`
- Processes joystick movement to determine directional states
- Sets press flags for up/down and left/right movements
- Handles threshold-based activation and deactivation

### 3. Button Handling Functions

#### `handleButtonPress(int pin, int& flag, uint8_t key, const char* action)`
- Generic button press/release handler
- Manages button state flags
- Sends keyboard press/release commands
- Provides debug output

#### `handleDirectionalKeys(JoystickData& joystick, uint8_t upKey, uint8_t downKey, uint8_t leftKey, uint8_t rightKey, int threshold)`
- Handles directional key presses based on joystick movement
- Manages key press/release states
- Prevents conflicting key combinations

#### `handleSprintKey(JoystickData& joystick, uint8_t sprintKey, int threshold, bool& active)`
- Handles sprint key activation based on joystick magnitude
- Uses hysteresis to prevent rapid on/off switching

### 4. Mouse Control Functions

#### `processMouseMovement(JoystickData& joystick, int sensitivity)`
- Converts joystick movement to mouse cursor movement
- Applies sensitivity scaling (inversion handled in readJoystick)
- Uses squared value for non-linear response

### 5. Key Release Management Functions

#### `releaseAllKeys()`
- Releases all keyboard keys used by the gamepad
- Centralized cleanup function

#### `releaseAllMouseButtons()`
- Releases all mouse buttons and stops cursor movement
- Centralized cleanup function

### 6. Utility Functions

#### `sgn(T val)` (Template)
- Returns sign of a value (-1, 0, or 1)
- Works with any numeric type

#### `calculateMagnitude(int x, int y)`
- Calculates magnitude of 2D vector
- Used for sprint threshold detection

## Benefits of Modular Architecture

### 1. Code Reusability
- Common operations are implemented once and reused
- Reduces code duplication from ~200 lines to ~50 lines in main loop

### 2. Maintainability
- Each function has a single responsibility
- Easy to modify specific functionality without affecting others
- Clear separation of concerns

### 3. Readability
- Main loop is now self-documenting with descriptive function calls
- Complex logic is encapsulated in well-named functions
- Reduced cognitive load when reading code

### 4. Testability
- Individual functions can be tested in isolation
- Easier to create unit tests for specific functionality
- Reduced dependencies between components

### 5. Extensibility
- Easy to add new joystick types or button configurations
- Simple to modify sensitivity or add new input methods
- Clear interfaces for future enhancements

## Usage Example

```cpp
// Initialize joysticks
initializeJoystick(leftJoystick, PIN_JOYSTICK_L_X, PIN_JOYSTICK_L_Y, PIN_JOYSTICK_L_SEL);
initializeJoystick(rightJoystick, PIN_JOYSTICK_R_X, PIN_JOYSTICK_R_Y, PIN_JOYSTICK_R_SEL);

// In main loop
readJoystick(leftJoystick);
readJoystick(rightJoystick);
processAxisMovement(leftJoystick, 200);
processMouseMovement(rightJoystick, sensitivityR, invertMouseR);
handleDirectionalKeys(leftJoystick, ACTION_JOYSTICK_L_UP, ACTION_JOYSTICK_L_DOWN, 
                     ACTION_JOYSTICK_L_LEFT, ACTION_JOYSTICK_L_RIGHT, 200);
```

## Configuration

The modular architecture maintains all existing configuration options while providing better organization:

- Pin assignments remain in `config.h`
- Sensitivity and threshold values are configurable
- Debug output is controlled by feature flags
- All existing functionality is preserved

This architecture provides a solid foundation for future enhancements while maintaining the simplicity and reliability of the original design.
