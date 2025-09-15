#ifndef COMPOSITE_HID_H
#define COMPOSITE_HID_H

#include <Arduino.h>

// CRITICAL: Include NicoHood HID library to ensure it takes priority
// over any HID functionality in the DFRobot LPUPS library
#include <HID-Project.h>
#include <HID-Settings.h>
#include "usb_config.h"

// Composite HID device that combines UPS Power Device, Mouse, and Keyboard functionality
// This implementation creates a properly structured composite device with separate interfaces
// for better Windows compatibility and ensures all HID functions are recognized

namespace CompositeHID {
  
  // Initialize the composite HID device
  void begin();
  
  // Power Device functions
  int sendPowerRemaining(uint8_t percentage);
  int sendPowerRuntime(uint16_t seconds);
  int sendPowerStatus(uint16_t status);
  
  // Mouse and Keyboard functions
  int sendMouseReport(int8_t x, int8_t y, uint8_t buttons, int8_t wheel = 0);
  int sendKeyboardReport(uint8_t modifiers, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t key6);
  
  // Mouse and Keyboard helper functions
  void moveMouse(int8_t x, int8_t y);
  void clickMouse(uint8_t button);
  void releaseMouse(uint8_t button);
  void pressKey(uint8_t key);
  void releaseKey(uint8_t key);
  void releaseAllKeys();
}

#endif // COMPOSITE_HID_H