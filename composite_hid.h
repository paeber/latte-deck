#ifndef COMPOSITE_HID_H
#define COMPOSITE_HID_H

#include <Arduino.h>
#include <HID-Project.h>
#include <HID-Settings.h>

// Composite HID device that combines UPS Power Device and Gamepad functionality
// This prevents HID descriptor conflicts by using a single unified descriptor

namespace CompositeHID {
  
  // Report IDs for different functionalities
  enum : uint8_t {
    // Power Device reports (UPS battery status)
    REPORT_ID_POWER_REMAINING = 1,
    REPORT_ID_POWER_RUNTIME = 2, 
    REPORT_ID_POWER_STATUS = 3,
    
    // Gamepad reports
    REPORT_ID_GAMEPAD = 4,
    REPORT_ID_MOUSE = 5,
    REPORT_ID_KEYBOARD = 6
  };

  // Initialize the composite HID device
  void begin();
  
  // Power Device functions
  int sendPowerRemaining(uint8_t percentage);
  int sendPowerRuntime(uint16_t seconds);
  int sendPowerStatus(uint16_t status);
  
  // Gamepad functions
  int sendGamepadReport(uint8_t buttons, int8_t x, int8_t y, int8_t z, int8_t rz);
  int sendMouseReport(int8_t x, int8_t y, uint8_t buttons);
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
