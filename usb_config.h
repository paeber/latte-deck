#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <Arduino.h>

// USB Device Descriptor Configuration for Composite HID Device
// This ensures Windows properly recognizes the device as a composite HID device

// Use conditional definitions to prevent redefinition errors with Arduino core
// These macros are likely already defined in USBCore.cpp in the Arduino core

// USB Device Descriptor - Use standard Arduino Leonardo IDs for better driver compatibility
#ifndef USB_VID
#define USB_VID 0x2341  // Arduino LLC Vendor ID
#endif

#ifndef USB_PID
#define USB_PID 0x8036  // Leonardo Product ID (use standard Leonardo PID for driver compatibility)
#endif

// USB Device Configuration - Use standard Arduino strings for better Windows recognition
// Define our custom string variables to avoid conflicts with Arduino core definitions
#define LATTE_USB_MANUFACTURER "Arduino LLC"
#define LATTE_USB_PRODUCT "Arduino Leonardo"
#define LATTE_USB_SERIAL "LATTEDECK1"

// USB Interface Configuration
// We need separate interfaces for each HID function
#define LATTE_USB_INTERFACE_COUNT 3  // Power Device + Mouse + Keyboard
#define LATTE_USB_ENDPOINT_COUNT 3   // One endpoint per interface

// USB Class and Subclass
#define LATTE_USB_CLASS_HID 0x03
#define LATTE_USB_SUBCLASS_NONE 0x00
#define LATTE_USB_PROTOCOL_NONE 0x00

// Report IDs for different functionalities
#define LATTE_REPORT_ID_POWER_REMAINING 1
#define LATTE_REPORT_ID_POWER_RUNTIME   2
#define LATTE_REPORT_ID_POWER_STATUS    3
#define LATTE_REPORT_ID_MOUSE           4
#define LATTE_REPORT_ID_KEYBOARD        5

#endif // USB_CONFIG_H