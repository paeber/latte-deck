# latte-deck
UPS and Gamepad HID for the LattePanda Leonardo

## Overview
A composite HID device that provides both UPS battery status reporting and gamepad functionality (mouse/keyboard emulation) for the LattePanda Leonardo board.

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
5. Select your LattePanda Leonardo board
6. Upload the sketch

## Hardware Requirements
- LattePanda Leonardo (ATmega32U4 based)
- DFRobot LPUPS module
- Gamepad hardware (joysticks, buttons)

## Architecture
The project uses a composite HID device approach to combine multiple HID functionalities:
- Power Device reports (battery status)
- Mouse reports (cursor movement, clicks, scroll wheel)
- Keyboard reports (key presses)

The joystick input is converted to mouse movement and keyboard key presses for better Windows compatibility. This prevents HID descriptor conflicts that would occur with separate HID devices.

## Documentation
See [docs/composite_hid_solution.md](docs/composite_hid_solution.md) for detailed technical information about the HID implementation.
