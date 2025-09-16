#include "composite_hid.h"
#include "usb_config.h"

using namespace CompositeHID;

// Power Device HID Descriptor - With report ID for Windows compatibility
static const uint8_t _powerDeviceDescriptor[] PROGMEM = {
  0x05, 0x84,             // Usage Page (Power Device)
  0x09, 0x01,             // Usage (Power Device)
  0xA1, 0x01,             // Collection (Application)
  
  // Report ID for Power Device
  0x85, LATTE_REPORT_ID_POWER_DEVICE, // Report ID
  
  // Battery Remaining Capacity
  0x09, 0x20,             // Usage (Battery Strength)
  0x15, 0x00,             // Logical Min 0
  0x26, 0x64, 0x00,       // Logical Max 100
  0x75, 0x08,             // Report Size 8
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  
  // Runtime to Empty
  0x09, 0x44,             // Usage (RunTimeToEmpty)
  0x15, 0x00,             // Logical Min 0
  0x26, 0xFF, 0x7F,       // Logical Max 32767
  0x75, 0x10,             // Report Size 16
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  
  // Present Status
  0x09, 0x16,             // Usage (PresentStatus)
  0x15, 0x00,             // Logical Min 0
  0x26, 0xFF, 0x7F,       // Logical Max 32767
  0x75, 0x10,             // Report Size 16
  0x95, 0x01,             // Report Count 1
  0x81, 0x02,             // Input (Data,Var,Abs)
  
  0xC0                    // End Collection
};

// Create HID sub-descriptor for Power Device only
// Mouse and Keyboard are handled by NicoHood library
static HIDSubDescriptor powerDeviceNode(_powerDeviceDescriptor, sizeof(_powerDeviceDescriptor));

void CompositeHID::begin()
{
  // Initialize NicoHood HID devices
  Mouse.begin();
  Keyboard.begin();
  
  // Register Power Device descriptor for UPS functionality
  HID().AppendDescriptor(&powerDeviceNode);
  
  // Give Windows time to enumerate the device
  delay(500);
  
  // Send initial Power Device report to establish UPS interface
  sendPowerReport(100, 3600, 0x0080); // 100%, 1 hour runtime, battery present
  delay(100);
}

// Power Device functions - Custom implementation for UPS
int CompositeHID::sendPowerReport(uint8_t percentage, uint16_t runtime, uint16_t status)
{
  // Create combined report structure matching HID descriptor
  // Report structure: [ReportID][BatteryStrength(8)][RuntimeToEmpty(16)][PresentStatus(16)]
  uint8_t report[6];
  report[0] = LATTE_REPORT_ID_POWER_DEVICE;  // Report ID
  report[1] = percentage;                    // Battery Strength (8-bit)
  report[2] = runtime & 0xFF;               // Runtime to Empty (16-bit, low byte)
  report[3] = (runtime >> 8) & 0xFF;        // Runtime to Empty (16-bit, high byte)
  report[4] = status & 0xFF;                // Present Status (16-bit, low byte)
  report[5] = (status >> 8) & 0xFF;         // Present Status (16-bit, high byte)
  
  return HID().SendReport(LATTE_REPORT_ID_POWER_DEVICE, report, sizeof(report));
}

// Mouse functions - Using NicoHood Mouse API
void CompositeHID::moveMouse(int8_t x, int8_t y)
{
  Mouse.move(x, y);
}

void CompositeHID::clickMouse(uint8_t button)
{
  Mouse.click(button);
}

void CompositeHID::pressMouse(uint8_t button)
{
  Mouse.press(button);
}

void CompositeHID::releaseMouse(uint8_t button)
{
  Mouse.release(button);
}

// Keyboard functions - Using NicoHood Keyboard API
void CompositeHID::pressKey(uint8_t key)
{
  Keyboard.press(key);
}

void CompositeHID::releaseKey(uint8_t key)
{
  Keyboard.release(key);
}

void CompositeHID::releaseAllKeys()
{
  Keyboard.releaseAll();
}

void CompositeHID::pressAndReleaseKey(uint8_t key)
{
  Keyboard.press(key);
  delay(10); // Small delay for key press
  Keyboard.release(key);
}