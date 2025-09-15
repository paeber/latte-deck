#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <Arduino.h>

// USB Device Descriptor Configuration for Composite HID Device
// This ensures Windows properly recognizes the device as a composite HID device

// USB Device Descriptor - Use standard Arduino Leonardo IDs for better driver compatibility
#define USB_VID 0x2341  // Arduino LLC Vendor ID
#define USB_PID 0x8036  // Leonardo Product ID (use standard Leonardo PID for driver compatibility)

// USB Device Configuration - Use standard Arduino strings for better Windows recognition
#define USB_MANUFACTURER_STRING "Arduino LLC"
#define USB_PRODUCT_STRING "Arduino Leonardo"
#define USB_SERIAL_STRING "8400310725"

// USB Interface Configuration
#define USB_INTERFACE_COUNT 4  // Multiple interfaces: CDC (Serial), Power Device, Mouse, Keyboard
#define USB_ENDPOINT_COUNT 4   // Multiple endpoints: CDC + one for each HID interface

// HID Configuration
#define HID_REPORT_DESCRIPTOR_SIZE 122  // Size of our composite HID descriptor (122 bytes)
#define HID_REPORT_SIZE 8               // Maximum report size

// USB Class and Subclass
#define USB_CLASS_HID 0x03
#define USB_SUBCLASS_NONE 0x00
#define USB_PROTOCOL_NONE 0x00

#endif // USB_CONFIG_H
