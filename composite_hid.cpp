#include "composite_hid.h"

using namespace CompositeHID;

// Separate HID Report Descriptors for better Windows compatibility
// This approach uses separate descriptors for each HID interface

// Power Device HID Descriptor
static const uint8_t _powerDeviceDescriptor[] PROGMEM = {
  0x05, 0x84,             // Usage Page (Power Device)
  0x09, 0x20,             // Usage (Battery Strength)
  0xA1, 0x01,             // Collection (Application)
  0x85, REPORT_ID_POWER_REMAINING,  // Report ID
  0x15, 0x00,             // Logical Min 0
  0x26, 0x64, 0x00,       // Logical Max 100
  0x75, 0x08,             // Report Size 8
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  0xC0,                   // End Collection
  
  0x05, 0x84,             // Usage Page (Power Device)
  0x09, 0x44,             // Usage (RunTimeToEmpty)
  0xA1, 0x01,             // Collection (Application)
  0x85, REPORT_ID_POWER_RUNTIME,    // Report ID
  0x15, 0x00,             // Logical Min 0
  0x26, 0xFF, 0x7F,       // Logical Max 32767
  0x75, 0x10,             // Report Size 16
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  0xC0,                   // End Collection
  
  0x05, 0x84,             // Usage Page (Power Device)
  0x09, 0x16,             // Usage (PresentStatus)
  0xA1, 0x01,             // Collection (Application)
  0x85, REPORT_ID_POWER_STATUS,     // Report ID
  0x15, 0x00,             // Logical Min 0
  0x26, 0xFF, 0x7F,       // Logical Max 32767
  0x75, 0x10,             // Report Size 16
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  0xC0                    // End Collection
};

// Mouse HID Descriptor
static const uint8_t _mouseDescriptor[] PROGMEM = {
  0x05, 0x01,             // Usage Page (Generic Desktop)
  0x09, 0x02,             // Usage (Mouse)
  0xA1, 0x01,             // Collection (Application)
  0x85, REPORT_ID_MOUSE,            // Report ID
  0x09, 0x01,             // Usage (Pointer)
  0xA1, 0x00,             // Collection (Physical)
  0x05, 0x09,             // Usage Page (Button)
  0x19, 0x01,             // Usage Minimum (Button 1)
  0x29, 0x03,             // Usage Maximum (Button 3)
  0x15, 0x00,             // Logical Minimum (0)
  0x25, 0x01,             // Logical Maximum (1)
  0x95, 0x03,             // Report Count (3)
  0x75, 0x01,             // Report Size (1)
  0x81, 0x02,             // Input (Data,Var,Abs)
  0x95, 0x01,             // Report Count (1)
  0x75, 0x05,             // Report Size (5)
  0x81, 0x01,             // Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,             // Usage Page (Generic Desktop)
  0x09, 0x30,             // Usage (X)
  0x09, 0x31,             // Usage (Y)
  0x09, 0x38,             // Usage (Wheel)
  0x15, 0x81,             // Logical Minimum (-127)
  0x25, 0x7F,             // Logical Maximum (127)
  0x75, 0x08,             // Report Size (8)
  0x95, 0x03,             // Report Count (3)
  0x81, 0x06,             // Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,                   // End Collection
  0xC0                    // End Collection
};

// Keyboard HID Descriptor
static const uint8_t _keyboardDescriptor[] PROGMEM = {
  0x05, 0x07,             // Usage Page (Keyboard/Keypad)
  0x09, 0x06,             // Usage (Keyboard)
  0xA1, 0x01,             // Collection (Application)
  0x85, REPORT_ID_KEYBOARD,         // Report ID
  0x05, 0x07,             // Usage Page (Keyboard/Keypad)
  0x19, 0xE0,             // Usage Minimum (224)
  0x29, 0xE7,             // Usage Maximum (231)
  0x15, 0x00,             // Logical Minimum (0)
  0x25, 0x01,             // Logical Maximum (1)
  0x75, 0x01,             // Report Size (1)
  0x95, 0x08,             // Report Count (8)
  0x81, 0x02,             // Input (Data,Var,Abs)
  0x95, 0x01,             // Report Count (1)
  0x75, 0x08,             // Report Size (8)
  0x81, 0x01,             // Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x06,             // Report Count (6)
  0x75, 0x08,             // Report Size (8)
  0x15, 0x00,             // Logical Minimum (0)
  0x25, 0x65,             // Logical Maximum (101)
  0x05, 0x07,             // Usage Page (Keyboard/Keypad)
  0x19, 0x00,             // Usage Minimum (0)
  0x29, 0x65,             // Usage Maximum (101)
  0x81, 0x00,             // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0                    // End Collection
};

// Create separate HID sub-descriptors for each interface
static HIDSubDescriptor powerDeviceNode(_powerDeviceDescriptor, sizeof(_powerDeviceDescriptor));
static HIDSubDescriptor mouseNode(_mouseDescriptor, sizeof(_mouseDescriptor));
static HIDSubDescriptor keyboardNode(_keyboardDescriptor, sizeof(_keyboardDescriptor));

void CompositeHID::begin()
{
  // Initialize USB HID with separate descriptors for better Windows compatibility
  // This approach registers each HID interface separately
  
  // Register each HID descriptor separately
  HID().AppendDescriptor(&powerDeviceNode);
  HID().AppendDescriptor(&mouseNode);
  HID().AppendDescriptor(&keyboardNode);
  
  // Give Windows time to enumerate the device
  delay(500);
  
  // Send initial reports to establish all interfaces
  // This helps Windows properly recognize each HID interface
  sendPowerRemaining(0);
  delay(50);
  sendPowerRuntime(0);
  delay(50);
  sendPowerStatus(0);
  delay(50);
  sendMouseReport(0, 0, 0, 0);
  delay(50);
  sendKeyboardReport(0, 0, 0, 0, 0, 0, 0);
  
  delay(100);
}

// Power Device functions
int CompositeHID::sendPowerRemaining(uint8_t percentage)
{
  return HID().SendReport(REPORT_ID_POWER_REMAINING, &percentage, sizeof(percentage));
}

int CompositeHID::sendPowerRuntime(uint16_t seconds)
{
  return HID().SendReport(REPORT_ID_POWER_RUNTIME, &seconds, sizeof(seconds));
}

int CompositeHID::sendPowerStatus(uint16_t status)
{
  return HID().SendReport(REPORT_ID_POWER_STATUS, &status, sizeof(status));
}

// Mouse functions
int CompositeHID::sendMouseReport(int8_t x, int8_t y, uint8_t buttons, int8_t wheel)
{
  uint8_t report[4];
  report[0] = buttons;
  report[1] = x;
  report[2] = y;
  report[3] = wheel;
  
  return HID().SendReport(REPORT_ID_MOUSE, report, sizeof(report));
}

int CompositeHID::sendKeyboardReport(uint8_t modifiers, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t key6)
{
  uint8_t report[8];
  report[0] = modifiers;
  report[1] = 0; // Reserved
  report[2] = key1;
  report[3] = key2;
  report[4] = key3;
  report[5] = key4;
  report[6] = key5;
  report[7] = key6;
  
  return HID().SendReport(REPORT_ID_KEYBOARD, report, sizeof(report));
}

// Mouse helper functions
void CompositeHID::moveMouse(int8_t x, int8_t y)
{
  sendMouseReport(x, y, 0, 0);
}

void CompositeHID::clickMouse(uint8_t button)
{
  sendMouseReport(0, 0, button, 0);
}

void CompositeHID::releaseMouse(uint8_t button)
{
  sendMouseReport(0, 0, 0, 0);
}

// Keyboard helper functions
void CompositeHID::pressKey(uint8_t key)
{
  sendKeyboardReport(0, key, 0, 0, 0, 0, 0);
}

void CompositeHID::releaseKey(uint8_t key)
{
  sendKeyboardReport(0, 0, 0, 0, 0, 0, 0);
}

void CompositeHID::releaseAllKeys()
{
  sendKeyboardReport(0, 0, 0, 0, 0, 0, 0);
}
