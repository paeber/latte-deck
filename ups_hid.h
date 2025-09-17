#ifndef UPS_HID_H
#define UPS_HID_H

#include <Arduino.h>
#include "config.h"
#include "ups_control.h"
#include "ups_power_device.h"

// ============================================================================
// HID Power Device Configuration
// ============================================================================

// Battery Status Constants
#define BATTERY_STATUS_UNKNOWN      0x00
#define BATTERY_STATUS_FULLY_CHARGED 0x01
#define BATTERY_STATUS_DISCHARGING  0x02
#define BATTERY_STATUS_CHARGING     0x03
#define BATTERY_STATUS_CRITICAL     0x04

// String indices for HID Power Device
#define IPRODUCT                     2
#define ISERIAL                      3
#define IMANUFACTURER                1
#define IDEVICECHEMISTRY             4
#define IOEMVENDOR                   5

// ============================================================================
// UPS HID Power Device Class
// ============================================================================

class UPSHID {
private:
    bool initialized;
    uint32_t last_report_time;
    uint16_t last_remaining_capacity;
    uint16_t last_present_status;
    uint16_t last_runtime_to_empty;
    
    // Internal methods
    void setupPowerDeviceFeatures();
    void updatePresentStatus(const UPSStatus& status);
    
public:
    UPSHID();
    
    // Initialization
    bool begin();
    
    // Main reporting function
    void reportBatteryStatus(const UPSStatus& status);
    
    // Debug functions
    void printStatus(const UPSStatus& status);
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