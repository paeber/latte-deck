#ifndef COMPOSITE_HID_H
#define COMPOSITE_HID_H

#include <Arduino.h>

// CRITICAL: Include NicoHood HID library to ensure it takes priority
// over any HID functionality in the DFRobot LPUPS library
#include <HID-Project.h>
#include <HID-Settings.h>

// Composite HID device that combines UPS Power Device and Gamepad functionality
// This prevents HID descriptor conflicts by using a single unified descriptor
// and ensures NicoHood HID library is used instead of DFRobot's HID implementation

namespace CompositeHID {
  
  // Report IDs for different functionalities
  enum : uint8_t {
    // Power Device reports (UPS battery status)
    REPORT_ID_POWER_REMAINING = 1,
    REPORT_ID_POWER_RUNTIME = 2, 
    REPORT_ID_POWER_STATUS = 3,
    
    // Mouse and Keyboard reports (no gamepad - Windows expects separate devices)
    REPORT_ID_MOUSE = 4,
    REPORT_ID_KEYBOARD = 5
  };

  // Initialize the composite HID device
  void begin();
  
  // Power Device functions
  int sendPowerRemaining(uint8_t percentage);
  int sendPowerRuntime(uint16_t seconds);
  int sendPowerStatus(uint16_t status);
  
  // Mouse and Keyboard functions (no gamepad - use mouse/keyboard directly)
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
