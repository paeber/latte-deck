# Configuration Reference

## Overview

This document provides a complete reference for all configuration options in the LatteDeck project. All configuration is centralized in `config.h` for easy management.

## Hardware Configuration

### UPS Hardware Settings

```cpp
// UPS I2C Communication
#define UPS_I2C_ADDRESS             0x55    // I2C address for UPS module
#define THREE_BATTERIES_UPS_PID     0x42AA  // Product ID for 3-battery UPS

// UPS Status LED
#define UPS_STATUS_LED              13      // Status LED pin

// Battery Configuration
#define N_CELLS_PACK                3       // Number of cells in series
#define MIN_CELL_VOLTAGE            2600    // Minimum cell voltage (mV)
#define MAX_CELL_VOLTAGE            4200    // Maximum cell voltage (mV)
```

### Gamepad Hardware Settings

```cpp
// Gamepad Enable Pin
#define PIN_GAMEPAD_ENABLE          4       // Pull LOW to enable gamepad

// Left Joystick Pins
#define PIN_JOYSTICK_L_X            A5      // Left joystick X-axis
#define PIN_JOYSTICK_L_Y            A4      // Left joystick Y-axis
#define PIN_JOYSTICK_L_SEL          2       // Left joystick button

// Right Joystick Pins
#define PIN_JOYSTICK_R_X            A3      // Right joystick X-axis
#define PIN_JOYSTICK_R_Y            A2      // Right joystick Y-axis
#define PIN_JOYSTICK_R_SEL          3       // Right joystick button

// Left Button Pins
#define PIN_BTN_L1                  5       // Left shoulder button
#define PIN_BTN_L2                  6       // Left top button
#define PIN_BTN_L3                  7       // Left middle button
#define PIN_BTN_L4                  8       // Left bottom button

// Right Button Pins
#define PIN_BTN_R1                  1       // Right shoulder button
#define PIN_BTN_R2                  4       // Right top button
#define PIN_BTN_R3                  9       // Right middle button
#define PIN_BTN_R4                  10      // Right bottom button
```

## Feature Flags

### Core Features

```cpp
// Enable mouse and keyboard emulation
#define ENABLE_MOUSE_KEYBOARD       1

// Enable UPS battery monitoring and HID Power Device
#define ENABLE_HID_POWER_DEVICE     1
```

### Debug Features

```cpp
// Enable UPS debug output (uncomment to enable)
//#define DEBUG_PRINT_UPS 1

// Enable gamepad debug output (uncomment to enable)
//#define DEBUG_PRINT_GAMEPAD 1
```

## Gamepad Configuration

### Joystick Settings

```cpp
// Joystick Sensitivity
#define JOYSTICK_SIDE_MAX           500     // Maximum joystick value
#define JOYSTICK_DEADZONE           50      // Deadzone for joystick center
#define JOYSTICK_MOUSE_SENSITIVITY  2       // Mouse movement sensitivity

// Joystick Inversion
#define JOYSTICK_L_INVERT_X         0       // Invert left joystick X-axis
#define JOYSTICK_L_INVERT_Y         0       // Invert left joystick Y-axis
#define JOYSTICK_R_INVERT_X         0       // Invert right joystick X-axis
#define JOYSTICK_R_INVERT_Y         0       // Invert right joystick Y-axis

// Movement Thresholds
#define MOVEMENT_THRESHOLD          200     // Threshold for directional movement
#define SPRINT_THRESHOLD            480     // Threshold for sprint activation
```

### Button Assignments

All button assignments are defined in `gamepad_assignment.h`:

```cpp
// Left Joystick Actions
#define ACTION_JOYSTICK_L_UP        'w'     // Move forward
#define ACTION_JOYSTICK_L_DOWN      's'     // Move backward
#define ACTION_JOYSTICK_L_LEFT      'a'     // Move left
#define ACTION_JOYSTICK_L_RIGHT     'd'     // Move right
#define ACTION_JOYSTICK_L_SEL       ' '     // Jump (space)
#define ACTION_JOYSTICK_L_MAX       'e'     // Sprint

// Right Joystick Actions
#define ACTION_JOYSTICK_R_UP        MOUSE_MOVE_UP
#define ACTION_JOYSTICK_R_DOWN      MOUSE_MOVE_DOWN
#define ACTION_JOYSTICK_R_LEFT      MOUSE_MOVE_LEFT
#define ACTION_JOYSTICK_R_RIGHT     MOUSE_MOVE_RIGHT
#define ACTION_JOYSTICK_R_SEL       MOUSE_LEFT
#define ACTION_JOYSTICK_R_MAX       ACTION_NONE

// Left Button Actions
#define ACTION_BTN_L1               MOUSE_RIGHT
#define ACTION_BTN_L2               'q'
#define ACTION_BTN_L3               '1'
#define ACTION_BTN_L4               '2'

// Right Button Actions
#define ACTION_BTN_R1               MOUSE_LEFT
#define ACTION_BTN_R2               ' '
#define ACTION_BTN_R3               'r'
#define ACTION_BTN_R4               'e'
```

### Available Action Types

```cpp
// Keyboard Keys
'w', 'a', 's', 'd', 'q', 'e', 'r', '1', '2', ' ', '\t', etc.

// Mouse Actions
MOUSE_LEFT          // Left mouse button
MOUSE_RIGHT         // Right mouse button
MOUSE_MIDDLE        // Middle mouse button

// Mouse Movement
MOUSE_MOVE_UP       // Move mouse up
MOUSE_MOVE_DOWN     // Move mouse down
MOUSE_MOVE_LEFT     // Move mouse left
MOUSE_MOVE_RIGHT    // Move mouse right

// Special Actions
ACTION_NONE         // No action (disable button)
```

## UPS Configuration

### Timing Settings

```cpp
// UPS Update Intervals
#define UPS_UPDATE_INTERVAL         1000    // Battery data update (ms)
#define UPS_LED_UPDATE_INTERVAL     50      // LED update interval (ms)
#define UPS_REPORT_INTERVAL         5000    // HID report interval (ms)
```

### Battery Calibration

```cpp
// Voltage-to-SoC Lookup Tables
// These are defined in ups_simple.cpp for different discharge rates
// 0.8A discharge rate (light load)
// 2.0A discharge rate (heavy load)

// Battery Status Thresholds
#define BATTERY_CRITICAL_PERCENT    10      // Critical battery level
#define BATTERY_LOW_PERCENT         20      // Low battery level
#define BATTERY_HIGH_PERCENT        80      // High battery level
```

### LED Patterns

```cpp
// LED Timing (defined in ups_simple.cpp)
#define LED_CHARGE_FADE_TIME        2000    // Charging fade cycle (ms)
#define LED_DISCHARGE_BLINK_BASE    100     // Base blink time (ms)
#define LED_DISCONNECT_BLINK        500     // Disconnect blink cycle (ms)
```

## Memory Configuration

### Buffer Sizes

```cpp
// Output and Debug Buffers
#define OUTPUT_BUFFER_SIZE          256     // Print buffer size
#define DEBUG_BUFFER_SIZE           80      // Debug buffer size
#define DATA_LEN_MAX                0x24U   // Maximum data length
```

### Memory Optimization

For memory-constrained environments, reduce buffer sizes:

```cpp
// Minimal configuration
#define OUTPUT_BUFFER_SIZE          128
#define DEBUG_BUFFER_SIZE           64
```

## HID Configuration

### Report IDs

```cpp
// HID Report IDs (defined in hid_config.h)
#define HID_POWER_DEVICE_REPORT_ID  1       // Power Device report ID
#define HID_MOUSE_REPORT_ID         2       // Mouse report ID
#define HID_KEYBOARD_REPORT_ID      3       // Keyboard report ID
```

### USB Configuration

```cpp
// USB Descriptor Settings (defined in usb_config.h)
#define USB_VID                     0x2341  // Arduino vendor ID
#define USB_PID                     0x8036  // Arduino Leonardo product ID
#define USB_MANUFACTURER_STRING     "LatteDeck"
#define USB_PRODUCT_STRING          "LatteDeck Gamepad"
```

## Customization Examples

### Disable Specific Buttons

```cpp
// Disable L4 button
#define ACTION_BTN_L4               ACTION_NONE

// Disable sprint functionality
#define ACTION_JOYSTICK_L_MAX       ACTION_NONE
```

### Change Button Mappings

```cpp
// Change L2 to F key
#define ACTION_BTN_L2               'f'

// Change R3 to Tab key
#define ACTION_BTN_R3               '\t'

// Change L1 to Shift key
#define ACTION_BTN_L1               KEY_LEFT_SHIFT
```

### Adjust Sensitivity

```cpp
// Increase mouse sensitivity
#define JOYSTICK_MOUSE_SENSITIVITY  3

// Decrease movement threshold
#define MOVEMENT_THRESHOLD          150

// Increase sprint threshold
#define SPRINT_THRESHOLD            500
```

### Invert Joystick Axes

```cpp
// Invert left joystick Y-axis (for different joystick orientation)
#define JOYSTICK_L_INVERT_Y         1

// Invert right joystick X-axis
#define JOYSTICK_R_INVERT_X         1
```

## Advanced Configuration

### Custom Battery Calibration

For different battery types, modify the voltage-to-SoC lookup tables in `ups_simple.cpp`:

```cpp
// Example for 2-cell battery pack
#define N_CELLS_PACK                2
#define MIN_CELL_VOLTAGE            3200    // 2S LiPo minimum
#define MAX_CELL_VOLTAGE            4200    // 2S LiPo maximum
```

### Custom LED Patterns

Modify LED behavior in `ups_simple.cpp`:

```cpp
// Custom LED timing
#define LED_CUSTOM_PATTERN_TIME     1000    // Custom pattern cycle
#define LED_CUSTOM_BLINK_COUNT      3       // Number of blinks
```

### Debug Configuration

Enable comprehensive debugging:

```cpp
// Enable all debug output
#define DEBUG_PRINT_UPS 1
#define DEBUG_PRINT_GAMEPAD 1

// Add custom debug messages
#ifdef DEBUG_PRINT_UPS
    Serial.println("Custom debug message");
#endif
```

## Configuration Validation

### Hardware Pin Conflicts

Ensure no pin conflicts in your configuration:

```cpp
// Check for duplicate pin assignments
// Each pin should only be used once
// Analog pins (A0-A5) can be used for digital I/O
// Digital pins 0,1 are used for Serial communication
```

### Memory Usage

Monitor memory usage with different configurations:

```cpp
// Check available RAM
Serial.print("Free RAM: ");
Serial.println(freeMemory());

// Adjust buffer sizes if needed
#define OUTPUT_BUFFER_SIZE          128     // Reduce if low on memory
#define DEBUG_BUFFER_SIZE           64      // Reduce if low on memory
```

### Feature Compatibility

Some features may conflict:

```cpp
// UPS and Gamepad can run simultaneously
#define ENABLE_HID_POWER_DEVICE     1
#define ENABLE_MOUSE_KEYBOARD       1

// Debug output may affect performance
//#define DEBUG_PRINT_UPS 1          // Comment out for production
//#define DEBUG_PRINT_GAMEPAD 1      // Comment out for production
```
