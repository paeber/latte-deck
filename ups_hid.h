#ifndef UPS_HID_H
#define UPS_HID_H

#include <Arduino.h>
#include <HID-Project.h>
#include "ups_control.h"
#include "hid_power_device.h"

// ============================================================================
// HID Power Device Configuration
// ============================================================================
// This module implements HID Power Device reporting for battery status
// to allow the operating system to display battery information

// ============================================================================
// HID Power Device Configuration
// ============================================================================

// HID Power Device Report ID (must match hid_config.h)
#define UPS_HID_REPORT_ID 1

// Battery Status Values
#define BATTERY_STATUS_UNKNOWN 0x00
#define BATTERY_STATUS_FULLY_CHARGED 0x01
#define BATTERY_STATUS_DISCHARGING 0x02
#define BATTERY_STATUS_CHARGING 0x03
#define BATTERY_STATUS_CRITICAL 0x04

// ============================================================================
// HID Power Device Class
// ============================================================================

class UPSHID {
private:
    HIDPowerDeviceReport last_report;
    
    // Internal methods
    void createReport(const UPSStatus& status);
    
public:
    UPSHID();
    
    // Initialization
    bool begin();
    
    // Main reporting function
    void reportBatteryStatus(const UPSStatus& status);
    
    // Debug functions
    void printReport(const HIDPowerDeviceReport& report);
};

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

extern UPSHID ups_hid;

// ============================================================================
// Function Prototypes
// ============================================================================

// HID Power Device functions
bool setupUPSHID();
void reportUPSBatteryStatus();

#endif // UPS_HID_H
