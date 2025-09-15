#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <Arduino.h>

// USB Device Descriptor Configuration for Composite HID Device
// This ensures Windows properly recognizes the device as a composite HID device

// USB Device Descriptor
#define USB_VID 0x2341  // Arduino LLC Vendor ID
#define USB_PID 0x8036  // Leonardo Product ID (or custom PID for composite device)

// USB Device Configuration
#define USB_MANUFACTURER_STRING "LatteDeck"
#define USB_PRODUCT_STRING "LatteDeck Composite HID"
#define USB_SERIAL_STRING "8400310725"

// USB Interface Configuration
#define USB_INTERFACE_COUNT 1  // Single interface for composite HID
#define USB_ENDPOINT_COUNT 1   // Single endpoint for HID reports

// HID Configuration
#define HID_REPORT_DESCRIPTOR_SIZE 122  // Size of our composite HID descriptor (122 bytes)
#define HID_REPORT_SIZE 8               // Maximum report size

// USB Class and Subclass
#define USB_CLASS_HID 0x03
#define USB_SUBCLASS_NONE 0x00
#define USB_PROTOCOL_NONE 0x00

#endif // USB_CONFIG_H
