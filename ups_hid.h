#ifndef UPS_HID_H
#define UPS_HID_H

#include <Arduino.h>
#include "hid_config.h"
#include <HID-Project.h>
#include <HID-Settings.h>

// UPS HID Power Device implementation that works with NicoHood HID library
// This provides battery reporting functionality without conflicts

namespace UPSHID {
  
  // Initialize UPS HID functionality
  void begin();
  
  // Send battery remaining capacity (0-100%)
  int sendRemainingCapacity(uint8_t percentage);
  
  // Send runtime to empty in seconds
  int sendRuntimeToEmpty(uint16_t runtime);
  
  // Send present status (bit flags)
  int sendPresentStatus(uint16_t status);
  
  // Send all battery data in one call
  int sendBatteryReport(uint8_t percentage, uint16_t runtime, uint16_t status);
}

#endif // UPS_HID_H
