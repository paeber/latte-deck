#ifndef HID_LEONARDO_H
#define HID_LEONARDO_H

#include <Arduino.h>

// ============================================================================
// Arduino Leonardo HID Implementation for Power Device
// ============================================================================
// This module provides a HID implementation that can send Power Device
// reports using the Arduino Leonardo's USB interface

// ============================================================================
// HID Report Structure for Power Device
// ============================================================================

struct LeonardoHIDPowerDeviceReport {
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
// Leonardo HID Class
// ============================================================================

class LeonardoHID {
private:
    bool initialized;
    uint32_t last_report_time;
    
    // Internal methods
    bool sendViaConsumerControl(const LeonardoHIDPowerDeviceReport& report);
    bool sendDirectUSBHIDReport(const uint8_t* data, uint8_t length);
    
public:
    LeonardoHID();
    
    // Initialization
    bool begin();
    bool isInitialized() const { return initialized; }
    
    // Power Device reporting
    void sendPowerDeviceReport(const LeonardoHIDPowerDeviceReport& report);
    
    // Debug functions
    void printReport(const LeonardoHIDPowerDeviceReport& report);
};

// ============================================================================
// Global Leonardo HID Instance
// ============================================================================

extern LeonardoHID leonardo_hid;

// ============================================================================
// Function Prototypes
// ============================================================================

// Leonardo HID functions
bool setupLeonardoHID();
void sendBatteryStatusLeonardo(const LeonardoHIDPowerDeviceReport& report);

#endif // HID_LEONARDO_H
