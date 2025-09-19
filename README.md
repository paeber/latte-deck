# LatteDeck
**Gamepad HID for LattePanda Delta 3**

A composite HID device that provides gamepad functionality (mouse/keyboard emulation) for the LattePanda Delta 3 using the embedded Arduino Leonardo (ATmega32U4).

## Features
- **Mouse and Keyboard Emulation**: Dual joystick input converted to mouse movement and keyboard key presses
- **Composite HID Device**: Single USB device with multiple HID interfaces (Mouse, Keyboard, Power Device)
- **UPS Battery Monitoring**: Real-time battery status reporting via JSON serial communication
- **Status LED Indication**: Visual feedback of battery status and charging state
- **OS Integration**: Native battery device integration for Ubuntu and other Linux distributions

## Quick Start

### Prerequisites
- LattePanda Delta 3 (with embedded Arduino Leonardo ATmega32U4)
- Gamepad hardware (joysticks, buttons)
- LattePanda UPS hat (3-cell configuration)

### Installation
1. **Install Libraries** via Arduino IDE Library Manager:
   - "HID-Project" by NicoHood
   - "DFRobot_LPUPS" by DFRobot

2. **Library Integration**:
   - The NicoHood HID library provides mouse and keyboard functionality
   - The DFRobot LPUPS library provides UPS battery monitoring

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
├── Power Device Interface (Report ID 1)
│   ├── Battery Capacity, Voltage, Current
│   ├── Temperature, Status, Runtime
├── Mouse Interface (Report ID 2)
│   ├── Buttons, X/Y Movement, Scroll Wheel
└── Keyboard Interface (Report ID 3)
    ├── Modifier Keys, LED Output, Key Array
```

### Key Components
- **`config.h`**: Centralized configuration (pins, constants, feature flags)
- **`gamepad.cpp`**: Main gamepad logic and state management
- **`gamepad_utils.cpp`**: Modular utility functions for joystick and button handling
- **`gamepad_utils.h`**: Utility function declarations and data structures

### UPS Module (Refactored)
- **`ups_simple.h/cpp`**: Simplified UPS functionality with JSON reporting
- **`DFRobot_LPUPS.h/cpp`**: DFRobot library integration for UPS communication

### Battery Service (OS Integration)
- **`battery-service/`**: Complete battery monitoring and OS integration
  - **Rust Implementation**: High-performance service for Ubuntu
  - **C++ Implementation**: Cross-platform service
  - **Python Scripts**: Monitoring and system integration
  - **Installation Scripts**: Automated deployment
  - **Documentation**: Complete deployment guides

## Configuration

### Feature Flags (in `config.h`)
```cpp
// Debug output
//#define DEBUG_PRINT_GAMEPAD 1
//#define DEBUG_PRINT_UPS 1

// Feature enable
#define ENABLE_MOUSE_KEYBOARD 1
#define ENABLE_HID_POWER_DEVICE 1
```

## Battery Monitoring

### Ubuntu Integration (Recommended)
The LatteDeck now includes a comprehensive battery monitoring system that integrates natively with Ubuntu:

```bash
# Quick installation
cd battery-service
sudo ./install_ubuntu.sh

# Check battery status
cat /sys/class/power_supply/latte_deck_battery/capacity
```

**Features:**
- **Native OS Integration**: Appears as system battery in Ubuntu
- **System Tray Display**: Battery percentage and charging status
- **High Performance**: Rust implementation uses only 2-8MB RAM
- **Real-time Monitoring**: JSON serial communication with Arduino
- **Automatic Service**: Runs as systemd service with auto-start

For complete documentation, see [`battery-service/README.md`](battery-service/README.md).

### Python Monitoring (Alternative)
For development and testing, Python scripts are available:

```bash
# Basic monitoring
python3 battery-service/battery_monitor.py /dev/ttyACM0 115200

# System integration
python3 battery-service/battery_system_integration.py /dev/ttyACM0 115200
```

## Troubleshooting

### Windows Issues
- **"This device cannot start (Code 10)"**: See [Troubleshooting Guide](docs/troubleshooting.md)
- **HID not recognized**: Check library installation and USB connection

### Ubuntu Battery Issues
- **Service not starting**: Check `sudo systemctl status latte-deck-battery`
- **No battery device**: Verify service logs with `sudo journalctl -u latte-deck-battery -f`
- **Serial port issues**: Test with `sudo minicom -D /dev/ttyACM0 -b 115200`

### Testing
1. **Device Manager**: Should show 3 HID devices without errors (Power Device, Mouse, Keyboard)
2. **Battery Status**: Ubuntu should show battery information in system tray
3. **Mouse**: Right joystick should move cursor
4. **Keyboard**: Left joystick should type WASD + Space
5. **Status LED**: Should indicate battery status and charging state

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
- [UPS Architecture](docs/ups_architecture.md) - Refactored UPS module structure and design
- [UPS Refactoring Plan](docs/ups_refactoring_plan.md) - Detailed refactoring analysis and benefits
- [Windows Battery Debugging](docs/windows_battery_debugging.md) - Comprehensive HID debugging guide
- [Modular Architecture](docs/modular_architecture.md) - Code organization and function reference
- [UPS Implementation](docs/ups_implementation.md) - UPS functionality and battery monitoring
- [Troubleshooting Guide](docs/troubleshooting.md) - Common issues and solutions
- [Windows HID Compatibility Fix](docs/windows_hid_compatibility_fix.md) - Technical details
- [Codebase Improvements](docs/codebase_improvements.md) - Development notes

## License
MIT License - See individual file headers for details.