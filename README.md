# LatteDeck
**UPS and Gamepad HID for LattePanda Delta 3**

A composite HID device that provides UPS battery status reporting and gamepad functionality (mouse/keyboard emulation) for the LattePanda Delta 3 using the embedded Arduino Leonardo (ATmega32U4).

## Features
- **UPS Battery Reporting**: Reports battery status, remaining capacity, and runtime to Windows
- **Mouse and Keyboard Emulation**: Dual joystick input converted to mouse movement and keyboard key presses
- **Composite HID Device**: Single USB device with three HID interfaces (Power Device, Mouse, Keyboard)

## Quick Start

### Prerequisites
- LattePanda Delta 3 (with embedded Arduino Leonardo ATmega32U4)
- DFRobot LPUPS module
- Gamepad hardware (joysticks, buttons)

### Installation
1. **Install Libraries** via Arduino IDE Library Manager:
   - "HID-Project" by NicoHood
   - "DFRobot_LPUPS" by DFRobot

2. **Library Integration**:
   - The DFRobot LPUPS library provides HID battery reporting functionality
   - The NicoHood HID library provides mouse and keyboard functionality
   - Both libraries work together without conflicts

3. **Upload Code**:
   - Clone this repository
   - Open `latte-deck.ino` in Arduino IDE
   - Select "Arduino Leonardo" board
   - Upload the sketch

## Hardware Configuration

### Pin Assignments
```cpp
// Gamepad Enable
PIN_GAMEPAD_ENABLE = 4

// Left Joystick (Keyboard)
PIN_JOYSTICK_L_X = A5, PIN_JOYSTICK_L_Y = A4, PIN_JOYSTICK_L_SEL = 21

// Right Joystick (Mouse)  
PIN_JOYSTICK_R_X = A2, PIN_JOYSTICK_R_Y = A1, PIN_JOYSTICK_R_SEL = 18

// UPS LEDs
UPS_RED_LED = 10, UPS_GREEN_LED = 9, UPS_BLUE_LED = 13
```

### Controls
- **Left Joystick**: WASD movement + Space (press), E (sprint)
- **Right Joystick**: Mouse cursor movement + Left click (press)
- **Gamepad Enable**: Pin 4 (pull-up) to enable/disable gamepad

## Architecture

### HID Interface Structure
```
USB Device
├── Power Device Interface (DFRobot LPUPS Library)
│   ├── Battery Remaining Capacity
│   ├── Runtime to Empty  
│   └── Present Status
├── Mouse Interface (NicoHood HID Library)
│   ├── Buttons, X/Y Movement, Scroll Wheel
└── Keyboard Interface (NicoHood HID Library)
    ├── Modifier Keys, LED Output, Key Array
```

### Key Components
- **`config.h`**: Centralized configuration (pins, constants, feature flags)
- **`ups_ctrl.cpp`**: UPS battery management and I2C communication using DFRobot library
- **`gamepad.cpp`**: Joystick input processing and HID output using NicoHood library
- **`ups_utils.cpp`**: UPS utility functions and HID initialization

## Configuration

### Feature Flags (in `config.h`)
```cpp
// Debug output
//#define DEBUG_PRINT_UPS 1
//#define DEBUG_PRINT_GAMEPAD 1

// Feature enable
#define ENABLE_MOUSE_KEYBOARD 1
#define ENABLE_HID_POWER_DEVICE 1
```

### UPS Settings
```cpp
#define UPS_I2C_ADDRESS 0x55
#define THREE_BATTERIES_UPS_PID 0x42AA
#define MIN_BATTERY_VOLTAGE 8000  // mV
#define MAX_BATTERY_VOLTAGE 12600 // mV
```

## Troubleshooting

### Windows Issues
- **"This device cannot start (Code 10)"**: See [Troubleshooting Guide](docs/troubleshooting.md)
- **HID not recognized**: Check library installation and USB connection
- **Battery not showing**: Check UPS I2C connection and PID value

### Testing
1. **Device Manager**: Should show 3 HID devices without errors
2. **Battery Status**: Windows Power settings should display battery
3. **Mouse**: Right joystick should move cursor
4. **Keyboard**: Left joystick should type WASD + Space

## Technical Notes

### Windows Compatibility
- Uses DFRobot LPUPS library's proven HID implementation for battery reporting
- Uses NicoHood HID library for mouse and keyboard functionality
- Both libraries work together without conflicts

### Memory Optimization
- Reduced buffer sizes (256B output, 80B debug)
- Configurable memory settings in `config.h`
- Efficient HID report structure

### Error Handling
- UPS communication retry logic (5 attempts)
- Graceful fallback if UPS initialization fails
- Comprehensive debug output options

## Documentation
- [DFRobot and NicoHood Integration](docs/dfrobot_nicohood_integration.md) - Library integration approach
- [Troubleshooting Guide](docs/troubleshooting.md) - Common issues and solutions
- [Windows HID Compatibility Fix](docs/windows_hid_compatibility_fix.md) - Technical details
- [Codebase Improvements](docs/codebase_improvements.md) - Development notes

## License
MIT License - See individual file headers for details.