#include "ups_hid.h"

using namespace UPSHID;

// UPS HID Power Device Descriptor
// This follows the USB HID Power Device specification for Windows compatibility
static const uint8_t _upsHidDescriptor[] PROGMEM = {
  0x05, 0x84,             // Usage Page (Power Device)
  0x09, 0x01,             // Usage (Power Device)
  0xA1, 0x01,             // Collection (Application)
  
  // Report ID for UPS Power Device
  0x85, 0x01,             // Report ID (1)
  
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

// Create HID sub-descriptor for UPS Power Device
static HIDSubDescriptor upsHidNode(_upsHidDescriptor, sizeof(_upsHidDescriptor));

void UPSHID::begin()
{
  // Register UPS Power Device descriptor
  HID().AppendDescriptor(&upsHidNode);
  
  // Give Windows time to enumerate the device
  delay(500);
  
  // Send initial battery report to establish UPS interface
  sendBatteryReport(100, 3600, 0x0080); // 100%, 1 hour runtime, battery present
  delay(100);
}

int UPSHID::sendRemainingCapacity(uint8_t percentage)
{
  return HID().SendReport(0x01, &percentage, sizeof(percentage));
}

int UPSHID::sendRuntimeToEmpty(uint16_t runtime)
{
  return HID().SendReport(0x01, &runtime, sizeof(runtime));
}

int UPSHID::sendPresentStatus(uint16_t status)
{
  return HID().SendReport(0x01, &status, sizeof(status));
}

int UPSHID::sendBatteryReport(uint8_t percentage, uint16_t runtime, uint16_t status)
{
  // Create combined report structure matching HID descriptor
  // Report structure: [ReportID][BatteryStrength(8)][RuntimeToEmpty(16)][PresentStatus(16)]
  uint8_t report[6];
  report[0] = 0x01;                    // Report ID
  report[1] = percentage;              // Battery Strength (8-bit)
  report[2] = runtime & 0xFF;         // Runtime to Empty (16-bit, low byte)
  report[3] = (runtime >> 8) & 0xFF;  // Runtime to Empty (16-bit, high byte)
  report[4] = status & 0xFF;          // Present Status (16-bit, low byte)
  report[5] = (status >> 8) & 0xFF;   // Present Status (16-bit, high byte)
  
  return HID().SendReport(0x01, report, sizeof(report));
}
