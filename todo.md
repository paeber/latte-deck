# TODO

## General
 - [ ] Cleanup code to a better maintainable structure. One master module for Gamepad functions and one master module for UPS. Both then need only one include each in the main sketch.

## UPS Battery System
 - [x] Find a way to report battery status to the operating system (Windows, Linux). Use USB-HID for that. Note that no conflict with the other HID functionalities occurs.
 - [x] Implement sending the HID report for the battery to the OS
 - [ ] Verify the reporting that the percentage, remaining time and charging are reflected in reasonable time
 - [x] Use the LED (D13) to represent Battery status
    - [ ] Charging: Fading on and off in 5 second (instead of 2) rythm
    - [ ] Discharging: Blink On/Off in 5 second (instead of 2) window while on time represents remaining percentage

## Gamepad
