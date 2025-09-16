#ifndef COMPOSITE_HID_H
#define COMPOSITE_HID_H

#include <Arduino.h>

// CRITICAL: Include NicoHood HID library to ensure it takes priority
// over any HID functionality in the DFRobot LPUPS library
#include <HID-Project.h>
#include <HID-Settings.h>
#include "usb_config.h"

// Composite HID device that combines UPS Power Device, Mouse, and Keyboard functionality
// This implementation uses NicoHood HID library for proper keyboard and mouse functionality
// while maintaining custom Power Device reporting for UPS battery status

namespace CompositeHID {
  
  // Initialize the composite HID device
  void begin();
  
  // Power Device functions (custom implementation for UPS)
  int sendPowerReport(uint8_t percentage, uint16_t runtime, uint16_t status);
  
  // Mouse functions (using NicoHood Mouse API)
  void moveMouse(int8_t x, int8_t y);
  void clickMouse(uint8_t button);
  void releaseMouse(uint8_t button);
  void pressMouse(uint8_t button);
  
  // Keyboard functions (using NicoHood Keyboard API)
  void pressKey(uint8_t key);
  void releaseKey(uint8_t key);
  void releaseAllKeys();
  void pressAndReleaseKey(uint8_t key);
}

#endif // COMPOSITE_HID_H