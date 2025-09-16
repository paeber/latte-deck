#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <Arduino.h>

// USB Device Descriptor Configuration for Composite HID Device
// This ensures Windows properly recognizes the device as a composite HID device

// For Arduino Leonardo, USB descriptor strings are set in the boards.txt file
// and can't be changed at runtime. The following definitions are used for
// HID report descriptors and other internal configurations.

// Standard Arduino Leonardo USB VID/PID
// These are defined in the Arduino core, but we include them here for reference
// and to ensure they're used in our HID descriptors
#ifndef USB_VID
#define USB_VID 0x2341  // Arduino LLC Vendor ID
#endif

#ifndef USB_PID
#define USB_PID 0x8036  // Leonardo Product ID
#endif

// These string constants are for reference only
// They can't be changed at runtime on Leonardo
#define LATTE_USB_MANUFACTURER "Arduino LLC"
#define LATTE_USB_PRODUCT "Arduino Leonardo"
#define LATTE_USB_SERIAL "LATTEDECK1"

// Report IDs for different HID functionalities
// Each top-level collection needs its own report ID for Windows compatibility
#define LATTE_REPORT_ID_MOUSE         2
#define LATTE_REPORT_ID_KEYBOARD      3

// HID interface configuration
// Used for internal reference only - actual USB configuration
// is handled by the Arduino core based on the board selection
#define LATTE_USB_INTERFACE_COUNT 2  // Mouse + Keyboard
#define LATTE_USB_ENDPOINT_COUNT 2   // One endpoint per interface

// HID class/subclass definitions
#define LATTE_USB_CLASS_HID 0x03
#define LATTE_USB_SUBCLASS_NONE 0x00
#define LATTE_USB_PROTOCOL_NONE 0x00

#endif // USB_CONFIG_H