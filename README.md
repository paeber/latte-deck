# LatteDeck
**Gamepad HID for LattePanda Delta 3**

A composite HID device that provides gamepad functionality (mouse/keyboard emulation) for the LattePanda Delta 3 using the embedded Arduino Leonardo (ATmega32U4).

## Features
- **Mouse and Keyboard Emulation**: Dual joystick input converted to mouse movement and keyboard key presses
- **Composite HID Device**: Single USB device with two HID interfaces (Mouse, Keyboard)

## Quick Start

### Prerequisites
- LattePanda Delta 3 (with embedded Arduino Leonardo ATmega32U4)
- Gamepad hardware (joysticks, buttons)

### Installation
1. **Install Libraries** via Arduino IDE Library Manager:
   - "HID-Project" by NicoHood

2. **Library Integration**:
   - The NicoHood HID library provides mouse and keyboard functionality

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

```

### Controls
- **Left Joystick**: WASD movement + Space (press), E (sprint)
- **Right Joystick**: Mouse cursor movement + Left click (press)
- **Gamepad Enable**: Pin 4 (pull-up) to enable/disable gamepad

## Architecture

### HID Interface Structure
```
USB Device
├── Mouse Interface (NicoHood HID Library)
│   ├── Buttons, X/Y Movement, Scroll Wheel
└── Keyboard Interface (NicoHood HID Library)
    ├── Modifier Keys, LED Output, Key Array
```

### Key Components
- **`config.h`**: Centralized configuration (pins, constants, feature flags)
- **`gamepad.cpp`**: Main gamepad logic and state management
- **`gamepad_utils.cpp`**: Modular utility functions for joystick and button handling
- **`gamepad_utils.h`**: Utility function declarations and data structures

## Configuration

### Feature Flags (in `config.h`)
```cpp
// Debug output
//#define DEBUG_PRINT_GAMEPAD 1

// Feature enable
#define ENABLE_MOUSE_KEYBOARD 1
```

## Troubleshooting

### Windows Issues
- **"This device cannot start (Code 10)"**: See [Troubleshooting Guide](docs/troubleshooting.md)
- **HID not recognized**: Check library installation and USB connection

### Testing
1. **Device Manager**: Should show 2 HID devices without errors
2. **Mouse**: Right joystick should move cursor
3. **Keyboard**: Left joystick should type WASD + Space

## Technical Notes

### Windows Compatibility
- Uses NicoHood HID library for mouse and keyboard functionality

### Memory Optimization
- Reduced buffer sizes (256B output, 80B debug)
- Configurable memory settings in `config.h`
- Efficient HID report structure

### Error Handling
- Comprehensive debug output options

## Documentation
- [Modular Architecture](docs/modular_architecture.md) - Code organization and function reference
- [Troubleshooting Guide](docs/troubleshooting.md) - Common issues and solutions
- [Windows HID Compatibility Fix](docs/windows_hid_compatibility_fix.md) - Technical details
- [Codebase Improvements](docs/codebase_improvements.md) - Development notes

## License
MIT License - See individual file headers for details.