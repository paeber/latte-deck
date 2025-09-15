#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <Arduino.h>

// USB Device Descriptor Configuration for Composite HID Device
// This ensures Windows properly recognizes the device as a composite HID device

// USB Device Descriptor - Use standard Arduino Leonardo IDs for better driver compatibility
#define USB_VID 0x2341  // Arduino LLC Vendor ID
#define USB_PID 0x8036  // Leonardo Product ID (use standard Leonardo PID for driver compatibility)

// USB Device Configuration - Use standard Arduino strings for better Windows recognition
#define USB_MANUFACTURER "Arduino LLC"
#define USB_PRODUCT "Arduino Leonardo"
#define USB_SERIAL "LATTEDECK1"

// USB Interface Configuration
// We need separate interfaces for each HID function
#define USB_INTERFACE_COUNT 3  // Power Device + Mouse + Keyboard
#define USB_ENDPOINT_COUNT 3   // One endpoint per interface

// USB Class and Subclass
#define USB_CLASS_HID 0x03
#define USB_SUBCLASS_NONE 0x00
#define USB_PROTOCOL_NONE 0x00

// Report IDs for different functionalities
#define REPORT_ID_POWER_REMAINING 1
#define REPORT_ID_POWER_RUNTIME   2
#define REPORT_ID_POWER_STATUS    3
#define REPORT_ID_MOUSE           4
#define REPORT_ID_KEYBOARD        5

#endif // USB_CONFIG_H