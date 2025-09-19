# LatteDeck Architecture

## Overview

LatteDeck is a gamepad controller for LattePanda Delta 3 with UPS battery monitoring. The project uses a modular architecture with clean separation of concerns.

## System Architecture

### Core Components

```
LatteDeck System
├── Gamepad Controller
│   ├── Joystick Input (WASD + Mouse)
│   ├── Button Mapping
│   └── HID Mouse/Keyboard
├── UPS Battery Monitor
│   ├── I2C Communication
│   ├── Battery Status LED
│   └── HID Power Device
└── Configuration Management
    ├── Centralized Config
    ├── Feature Flags
    └── Hardware Definitions
```

### File Structure

```
latte-deck/
├── latte-deck.ino          # Main application
├── config.h                # Centralized configuration
├── gamepad.h/cpp           # Gamepad logic
├── gamepad_utils.h/cpp     # Gamepad utilities
├── gamepad_assignment.h    # Button mappings
├── gamepad_pinout.h        # Hardware pin definitions
├── ups_simple.h/cpp        # UPS battery monitoring
├── hid_config.h            # HID configuration
└── usb_config.h            # USB descriptor configuration
```

## Gamepad Architecture

### Modular Design

The gamepad system uses a modular architecture that eliminates code duplication:

#### Core Data Structure
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

#### Key Functions
- `initializeJoystick()` - Setup joystick data structure
- `readJoystick()` - Read analog values with calibration
- `processAxisMovement()` - Handle directional key presses
- `processMouseMovement()` - Convert joystick to mouse movement
- `handleButtonPress()` - Generic button handler

### Button Assignments

| Component | Action | Key/Function | Description |
|-----------|--------|--------------|-------------|
| **Left Joystick** | | | |
| | Up | w | Move forward |
| | Down | s | Move backward |
| | Left | a | Move left |
| | Right | d | Move right |
| | Press | Space | Jump |
| | Max (Sprint) | e | Sprint when pushed to edge |
| **Left Buttons** | | | |
| | L1 (Shoulder) | Right Mouse Click | Secondary action |
| | L2 (Top) | Q | Quick action |
| | L3 (Middle) | 1 | Hotkey 1 |
| | L4 (Bottom) | 2 | Hotkey 2 |
| **Right Joystick** | | | |
| | Up/Down/Left/Right | Mouse Movement | Mouse cursor control |
| | Press | Left Mouse Click | Primary mouse click |
| **Right Buttons** | | | |
| | R1 (Shoulder) | Left Mouse Click | Primary mouse click |
| | R2 (Top) | Space | Jump |
| | R3 (Middle) | R | Reload/Interact |
| | R4 (Bottom) | E | Use/Interact |

## UPS Architecture

### Simplified Design

The UPS system has been streamlined from 10 complex files to 2 simple files:

#### SimpleUPS Class
```cpp
class SimpleUPS {
public:
    bool begin();
    void update();
    uint16_t getCapacityPercent();
    uint16_t getVoltage();
    bool isCharging();
    bool isConnected();
};
```

#### Key Features
- **Non-blocking operation** - Runs in background
- **I2C communication** with DFRobot LPUPS library
- **Status LED indication** - Visual battery feedback
- **HID Power Device** - Windows battery reporting
- **Robust error handling** - Graceful degradation

### Battery Monitoring

#### Hardware Configuration
- **I2C Address**: 0x55
- **Product ID**: 0x42AA (3-cell UPS)
- **Status LED**: Pin 13
- **Battery Specs**: 3 cells, 9.2V-12.6V, 12Ah total

#### Status LED Behavior
- **Charging**: Fading in/out every 2 seconds
- **Discharging**: Blink pattern (on-time = battery %)
- **Disconnected**: Fast blink (500ms cycle)

## HID Implementation

### Composite Device Structure

The device presents as a composite USB HID device with three interfaces:

```
USB Device
├── Power Device Interface (Report ID 1)
│   ├── Battery Remaining Capacity
│   ├── Runtime to Empty
│   └── Present Status
├── Mouse Interface (Report ID 2)
│   ├── Buttons
│   ├── X/Y Movement
│   └── Scroll Wheel
└── Keyboard Interface (Report ID 3)
    ├── Modifier Keys
    ├── LED Output
    └── Key Array
```

### Windows Compatibility

Proper report IDs ensure Windows compatibility:
- Each top-level collection has unique report ID
- Eliminates "top level collection" errors
- Proper device enumeration in Device Manager

## Configuration Management

### Centralized Configuration

All configuration is centralized in `config.h`:

#### Hardware Configuration
```cpp
// UPS Hardware
#define UPS_I2C_ADDRESS             0x55
#define THREE_BATTERIES_UPS_PID     0x42AA
#define UPS_STATUS_LED              13

// Gamepad Pins
#define PIN_JOYSTICK_L_X            A5
#define PIN_JOYSTICK_L_Y            A4
// ... etc
```

#### Feature Flags
```cpp
#define ENABLE_MOUSE_KEYBOARD       1
#define ENABLE_HID_POWER_DEVICE     1
//#define DEBUG_PRINT_UPS 1
//#define DEBUG_PRINT_GAMEPAD 1
```

#### Memory Configuration
```cpp
#define OUTPUT_BUFFER_SIZE          256
#define DEBUG_BUFFER_SIZE           80
```

## Key Improvements

### 1. **Hardware Reliability**
- Fixed pin conflicts (PIN_BTN_R1/R2 both used pin 1)
- Added missing constants (UPS_I2C_ADDRESS, THREE_BATTERIES_UPS_PID)
- Standardized preprocessor directives

### 2. **Memory Efficiency**
- Reduced RAM usage by ~300 bytes
- Configurable buffer sizes
- Eliminated memory leaks in UPS system

### 3. **Code Quality**
- Modular architecture eliminates duplication
- Centralized configuration management
- Improved error handling and debugging

### 4. **Windows Compatibility**
- Proper HID report IDs for composite device
- Eliminated Windows HID parser errors
- Reliable battery status reporting

## Benefits

1. **Maintainability** - Clear separation of concerns, modular design
2. **Reliability** - Fixed hardware conflicts, robust error handling
3. **Performance** - Reduced memory usage, non-blocking operations
4. **Compatibility** - Windows HID compliance, proper device enumeration
5. **Extensibility** - Easy to add features, configurable architecture
