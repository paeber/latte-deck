#ifndef HID_POWER_DEVICE_H
#define HID_POWER_DEVICE_H

#include <Arduino.h>
#include "hid_descriptor.h"

// ============================================================================
// HID Power Device Implementation
// ============================================================================
// This module provides a custom HID Power Device implementation for UPS battery reporting
// It works alongside the existing HID-Project library for Mouse and Keyboard

// ============================================================================
// HID Power Device Report Structure
// ============================================================================

struct HIDPowerDeviceReport {
    uint8_t report_id;           // Report ID (1)
    uint8_t battery_present;     // 0 = not present, 1 = present
    uint8_t battery_capacity;    // Battery capacity percentage (0-100)
    uint16_t battery_voltage;    // Battery voltage in millivolts
    int16_t battery_current;     // Battery current in milliamps (negative = discharging)
    uint8_t battery_temperature; // Battery temperature in Celsius
    uint8_t battery_status;      // Battery status (charging, discharging, etc.)
    uint16_t runtime_to_empty;   // Estimated runtime to empty in minutes
} __attribute__((packed));

// ============================================================================
// HID Power Device Class
// ============================================================================

class HIDPowerDevice {
private:
    HIDPowerDeviceReport last_report;
    bool report_sent;
    uint32_t last_report_time;
    
    // Internal methods
    bool sendRawHIDReport(const uint8_t* data, uint8_t length);
    
public:
    HIDPowerDevice();
    
    // Initialization
    bool begin();
    
    // Main reporting function
    void reportBatteryStatus(const HIDPowerDeviceReport& report);
    
    // Debug functions
    void printReport(const HIDPowerDeviceReport& report);
    
    // Get last report
    const HIDPowerDeviceReport& getLastReport() const { return last_report; }
};

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

extern HIDPowerDevice hid_power_device;

// ============================================================================
// Function Prototypes
// ============================================================================

// HID Power Device functions
bool setupHIDPowerDevice();
void reportBatteryStatusHID(const HIDPowerDeviceReport& report);

#endif // HID_POWER_DEVICE_H
