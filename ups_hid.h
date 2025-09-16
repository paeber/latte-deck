#ifndef UPS_HID_H
#define UPS_HID_H

#include <Arduino.h>
#include <HID-Project.h>
#include "ups_control.h"

// ============================================================================
// HID Power Device Configuration
// ============================================================================
// This module implements HID Power Device reporting for battery status
// to allow the operating system to display battery information

// ============================================================================
// HID Power Device Report Structure
// ============================================================================

// HID Power Device Report ID (must match hid_config.h)
#define UPS_HID_REPORT_ID 1

// HID Power Device Usage Pages
#define HID_USAGE_PAGE_POWER_DEVICE 0x84
#define HID_USAGE_PAGE_BATTERY_SYSTEM 0x85

// HID Power Device Usage IDs
#define HID_USAGE_BATTERY_PRESENT 0x66
#define HID_USAGE_BATTERY_CAPACITY 0x66
#define HID_USAGE_BATTERY_VOLTAGE 0x30
#define HID_USAGE_BATTERY_CURRENT 0x31
#define HID_USAGE_BATTERY_TEMPERATURE 0x32
#define HID_USAGE_BATTERY_STATUS 0x33
#define HID_USAGE_BATTERY_RUNTIME_TO_EMPTY 0x34

// Battery Status Values
#define BATTERY_STATUS_UNKNOWN 0x00
#define BATTERY_STATUS_FULLY_CHARGED 0x01
#define BATTERY_STATUS_DISCHARGING 0x02
#define BATTERY_STATUS_CHARGING 0x03
#define BATTERY_STATUS_CRITICAL 0x04

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

class UPSHID {
private:
    HIDPowerDeviceReport last_report;
    bool report_sent;
    uint32_t last_report_time;
    
    // Internal methods
    void createReport(const UPSStatus& status);
    bool sendReport();
    
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
