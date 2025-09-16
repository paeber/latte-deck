#ifndef HID_DESCRIPTOR_H
#define HID_DESCRIPTOR_H

#include <Arduino.h>

// ============================================================================
// HID Report Descriptor for Composite Device
// ============================================================================
// This descriptor defines a composite HID device with:
// - Report ID 1: Power Device (UPS Battery Status)
// - Report ID 2: Mouse
// - Report ID 3: Keyboard
//
// The descriptor is designed to work with Windows and Linux without conflicts

// HID Report Descriptor for Power Device (UPS Battery Status)
// Based on HID Usage Tables for USB Power Device Class
const uint8_t HID_REPORT_DESCRIPTOR[] PROGMEM = {
    // ============================================================================
    // Power Device Collection (Report ID 1)
    // ============================================================================
    0x05, 0x84,        // Usage Page (Power Device)
    0x09, 0x04,        // Usage (UPS)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    
    // Battery Present
    0x05, 0x85,        //   Usage Page (Battery System)
    0x09, 0x66,        //   Usage (Battery Present)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Capacity (0-100%)
    0x09, 0x66,        //   Usage (Battery Capacity)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x64, 0x00,  //   Logical Maximum (100)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Voltage (0-65535 mV)
    0x09, 0x30,        //   Usage (Battery Voltage)
    0x15, 0x00,        //   Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65535)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Current (-32768 to 32767 mA)
    0x09, 0x31,        //   Usage (Battery Current)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Temperature (0-255°C)
    0x09, 0x32,        //   Usage (Battery Temperature)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Status
    0x09, 0x33,        //   Usage (Battery Status)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x04,        //   Logical Maximum (4)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Runtime to Empty (0-65535 minutes)
    0x09, 0x34,        //   Usage (Runtime to Empty)
    0x15, 0x00,        //   Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65535)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Padding to align to byte boundary
    0x75, 0x07,        //   Report Size (7)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Constant, Variable, Absolute)
    
    0xC0,              // End Collection
    
    // ============================================================================
    // Mouse Collection (Report ID 2)
    // ============================================================================
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x03,        //     Usage Maximum (0x03)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data, Variable, Absolute)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x05,        //     Report Size (5)
    0x81, 0x01,        //     Input (Constant)
    0x05, 0x01,        //     Usage Page (Generic Desktop)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data, Variable, Relative)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    
    // ============================================================================
    // Keyboard Collection (Report ID 3)
    // ============================================================================
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x05, 0x07,        //   Usage Page (Key Codes)
    0x19, 0xE0,        //   Usage Minimum (224)
    0x29, 0xE7,        //   Usage Maximum (231)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Constant)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0x65,        //   Usage Maximum (101)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (101)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x00,        //   Input (Data, Array)
    0xC0               // End Collection
};

// Descriptor size
#define HID_REPORT_DESCRIPTOR_SIZE sizeof(HID_REPORT_DESCRIPTOR)

// ============================================================================
// HID Report Sizes
// ============================================================================

// Power Device report size (Report ID + 8 bytes of data)
#define HID_POWER_DEVICE_REPORT_SIZE 9

// Mouse report size (Report ID + 3 bytes of data)
#define HID_MOUSE_REPORT_SIZE 4

// Keyboard report size (Report ID + 8 bytes of data)
#define HID_KEYBOARD_REPORT_SIZE 9

#endif // HID_DESCRIPTOR_H
