# latte-deck
UPS and Gamepad HID for the LattePanda Leonardo

## Overview
A composite HID device that provides both UPS battery status reporting and gamepad functionality (mouse/keyboard emulation) for the LattePanda Leonardo board.

## Features
- **UPS Battery Reporting**: Reports battery status, remaining capacity, and runtime to the operating system
- **Gamepad Functionality**: Dual joystick gamepad with mouse and keyboard emulation
- **Composite HID Device**: Single USB device with multiple HID interfaces to prevent conflicts

## Dependencies

### Required Libraries
- **[NicoHood HID-Project](https://github.com/NicoHood/HID)**: Enhanced HID functionality for Arduino
- **[DFRobot LPUPS](https://github.com/DFRobot/DFRobot_LPUPS)**: UPS hardware communication library

### Installation
1. Install the required libraries through Arduino IDE Library Manager:
   - Search for "HID-Project" by NicoHood
   - Search for "DFRobot_LPUPS" by DFRobot
2. Clone this repository to your Arduino sketches folder
3. Open `latte-deck.ino` in Arduino IDE
4. Select your LattePanda Leonardo board
5. Upload the sketch

## Hardware Requirements
- LattePanda Leonardo (ATmega32U4 based)
- DFRobot LPUPS module
- Gamepad hardware (joysticks, buttons)

## Architecture
The project uses a composite HID device approach to combine multiple HID functionalities:
- Power Device reports (battery status)
- Gamepad reports (joystick input)
- Mouse reports (cursor movement)
- Keyboard reports (key presses)

This prevents HID descriptor conflicts that would occur with separate HID devices.

## Documentation
See [docs/composite_hid_solution.md](docs/composite_hid_solution.md) for detailed technical information about the HID implementation.
