#ifndef UPS_CONTROL_H
#define UPS_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include "ups.h"

// ============================================================================
// UPS Control Module
// ============================================================================
// This module provides UPS control functionality using the DFRobot LPUPS library
// for reading battery status and reporting it via HID Power Device interface

// ============================================================================
// UPS Status Structure
// ============================================================================

struct UPSStatus {
    uint16_t voltage_mV;           // Battery voltage in millivolts
    uint16_t current_mA;           // Battery current in milliamps
    uint16_t capacity_percent;     // Battery capacity percentage (0-100)
    uint16_t temperature_celsius;  // Battery temperature in Celsius
    bool is_charging;              // True if battery is charging
    bool is_connected;             // True if UPS is connected and responding
    uint32_t last_update_ms;       // Timestamp of last successful update
};

// ============================================================================
// UPS Control Class
// ============================================================================

class UPSControl {
private:
    UPSStatus status;
    bool initialized;
    uint32_t last_report_ms;
    uint32_t last_led_update_ms;
    uint8_t led_state;
    
    // Internal methods
    bool readUPSData();
    void updateStatusLED();
    void printDebugInfo();
    
public:
    UPSControl();
    
    // Initialization
    bool begin();
    bool isInitialized() const { return initialized; }
    
    // Status access
    const UPSStatus& getStatus() const { return status; }
    uint16_t getCapacityPercent() const { return status.capacity_percent; }
    uint16_t getVoltage() const { return status.voltage_mV; }
    bool isCharging() const { return status.is_charging; }
    bool isConnected() const { return status.is_connected; }
    
    // Main loop function (non-blocking)
    void update();
    
    // HID reporting
    void reportBatteryStatus();
    
    // Debug output
    void printStatus();
};

// ============================================================================
// Global UPS Control Instance
// ============================================================================

extern UPSControl ups;

// ============================================================================
// Function Prototypes
// ============================================================================

// UPS initialization and control
bool setupUPS();
void loopUPS();

// Debug functions
void printUPS(const char* msg);
void printUPS(const String& msg);
void printUPSF(const char* format, ...);

#endif // UPS_CONTROL_H
