# Arduino Leonardo USB Descriptor Limitations

This document explains the USB descriptor limitations on the Arduino Leonardo board and how they affect the Latte-Deck project.

## USB Descriptor Handling on Leonardo

The Arduino Leonardo (ATmega32U4) has specific limitations regarding USB descriptor handling:

1. **Fixed USB Descriptors**: The USB device descriptors (Manufacturer, Product, Serial Number) are set at compile time and cannot be changed at runtime.

2. **No `USBDevice` Methods**: Unlike some other Arduino boards with USB capabilities, the Leonardo does not support the `USBDevice.setManufacturer()`, `USBDevice.setProduct()`, and `USBDevice.setSerialNumber()` methods.

3. **Board Configuration**: USB descriptors are configured through the Arduino IDE's boards.txt file and the Arduino core libraries.

## How USB Descriptors Work on Leonardo

On the Leonardo, USB descriptors are handled as follows:

1. **VID/PID**: The Vendor ID (VID) and Product ID (PID) are defined in the Arduino core and are set to:
   - VID: 0x2341 (Arduino LLC)
   - PID: 0x8036 (Leonardo)

2. **String Descriptors**: The manufacturer, product, and serial number strings are also defined in the Arduino core.

3. **HID Descriptors**: While the device descriptors are fixed, the HID report descriptors can be customized, which is what we do in the Latte-Deck project.

## Impact on Latte-Deck Project

For the Latte-Deck project, this means:

1. **Device Recognition**: Windows will always see the device as "Arduino Leonardo" rather than a custom name.

2. **HID Functionality**: Despite this limitation, our custom HID descriptors (for Power Device, Mouse, and Keyboard) still work correctly.

3. **Code Adaptations**: We've removed calls to the unsupported `USBDevice` methods and rely on the default Leonardo USB configuration.

## Workarounds for Custom USB Descriptors

If custom USB device descriptors are required, consider these options:

1. **Modify boards.txt**: Edit the Arduino IDE's boards.txt file to change the default Leonardo USB descriptors.
   - Location: `<Arduino installation>/hardware/arduino/avr/boards.txt`
   - Find the Leonardo section and modify the USB-related entries
   - This affects all Leonardo projects

2. **Custom Arduino Core**: Create a custom version of the Arduino core with modified USB descriptors.
   - This is more complex but allows for project-specific USB descriptors
   - Requires maintaining a custom Arduino core

3. **Alternative Board**: Use a different board that supports runtime USB descriptor changes, such as:
   - Teensy boards
   - Some ARM-based boards (e.g., Arduino Due)
   - ESP32 boards with USB capabilities

## Current Implementation

In the current implementation:

1. We define USB descriptor constants in `usb_config.h` for reference and documentation purposes.
2. We use these constants in our HID report descriptors and report sending functions.
3. We do not attempt to change the device-level USB descriptors at runtime.

This approach ensures compatibility with the Arduino Leonardo while still providing the required HID functionality.

## Testing USB Recognition

To verify that Windows recognizes the device correctly:

1. Connect the Latte-Deck to a Windows computer
2. Open Device Manager
3. Look for "Arduino Leonardo" under "Ports (COM & LPT)" and "Human Interface Devices"
4. Verify that there are no warning icons or error codes

If Windows shows "Unknown Device" or error codes, refer to the [Windows Driver Troubleshooting Guide](windows_driver_troubleshooting.md).
