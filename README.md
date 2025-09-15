# latte-deck
UPS and Gamepad HID for the LattePanda Delta 3

## Overview
A composite HID device that provides both UPS battery status reporting and gamepad functionality (mouse/keyboard emulation) for the LattePanda Delta 3. The device uses the embedded Arduino Leonardo (ATmega32U4) to create a unified HID interface that reports battery status to the operating system while providing gamepad controls through mouse and keyboard emulation.

## Features
- **UPS Battery Reporting**: Reports battery status, remaining capacity, and runtime to the operating system
- **Mouse and Keyboard Emulation**: Dual joystick input converted to mouse movement and keyboard key presses
- **Composite HID Device**: Single USB device with multiple HID interfaces to prevent conflicts

## Dependencies

### Required Libraries
- **[NicoHood HID-Project](https://github.com/NicoHood/HID)**: Enhanced HID functionality for Arduino
- **[DFRobot LPUPS](https://github.com/DFRobot/DFRobot_LPUPS)**: UPS hardware communication library

### Installation
1. Install the required libraries through Arduino IDE Library Manager:
   - Search for "HID-Project" by NicoHood
   - Search for "DFRobot_LPUPS" by DFRobot
2. **Important**: Remove conflicting HID files from DFRobot library:
   - Navigate to your Arduino libraries folder (usually `~/Arduino/libraries/DFRobot_LPUPS/`)
   - Delete or rename `HID.c` and `HID.h` files to prevent conflicts with NicoHood HID library
   - This is necessary because DFRobot library includes its own HID implementation that conflicts with NicoHood's
3. Clone this repository to your Arduino sketches folder
4. Open `latte-deck.ino` in Arduino IDE
5. Select your LattePanda Delta 3 board (Arduino Leonardo)
6. Upload the sketch

## Hardware Requirements
- LattePanda Delta 3 (with embedded Arduino Leonardo ATmega32U4)
- DFRobot LPUPS module
- Gamepad hardware (joysticks, buttons)

## Architecture
The project uses a composite HID device approach to combine multiple HID functionalities:
- Power Device reports (battery status)
- Mouse reports (cursor movement, clicks, scroll wheel)
- Keyboard reports (key presses)

The joystick input is converted to mouse movement and keyboard key presses for better Windows compatibility. This prevents HID descriptor conflicts that would occur with separate HID devices.

## Documentation
- [docs/composite_hid_solution.md](docs/composite_hid_solution.md) - Detailed technical information about the HID implementation
- [docs/windows_driver_troubleshooting.md](docs/windows_driver_troubleshooting.md) - Windows driver installation and troubleshooting guide
- [docs/hid_testing_guide.md](docs/hid_testing_guide.md) - Guide for testing all HID interfaces

## HID Implementation
The latest version implements a true composite HID device with three separate interfaces:
1. **Power Device Interface**: Reports battery status, charge level, and runtime to Windows
2. **Mouse Interface**: Standard HID mouse with relative X/Y movement, buttons, and scroll wheel
3. **Keyboard Interface**: Standard HID keyboard with modifier keys and 6-key rollover

This implementation ensures all three interfaces are properly recognized by Windows without conflicts. Each interface uses a unique report ID and follows standard HID specifications for maximum compatibility.

## Troubleshooting
If you encounter "This device cannot start. (Code 10)" errors in Windows Device Manager, see the [Windows Driver Troubleshooting Guide](docs/windows_driver_troubleshooting.md) for step-by-step solutions.

To verify that all interfaces are working correctly, follow the [HID Testing Guide](docs/hid_testing_guide.md).
