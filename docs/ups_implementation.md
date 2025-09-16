# UPS Implementation Documentation

## Overview

The LatteDeck project now includes comprehensive UPS (Uninterruptible Power Supply) functionality for the LattePanda Delta 3 UPS hat. This implementation provides real-time battery monitoring and reporting to the operating system via USB HID Power Device interface.

## Features

- **Real-time Battery Monitoring**: Continuous monitoring of battery voltage, current, capacity, and temperature
- **HID Power Device Reporting**: Battery status reported to Windows/Linux via USB HID
- **Status LED Indication**: Visual feedback of battery status and charging state
- **Non-blocking Operation**: UPS monitoring runs in background without affecting gamepad functionality
- **DFRobot Library Integration**: Uses the official DFRobot LPUPS library for hardware communication

## Architecture

### Core Components

1. **`ups_control.h/cpp`** - Main UPS control module
2. **`ups_hid.h/cpp`** - HID Power Device reporting
3. **`ups.h/cpp`** - Battery voltage-to-SoC calculation utilities
4. **`config.h`** - Configuration and feature flags

### Data Flow

```
UPS Hardware (I2C) → DFRobot Library → UPSControl → UPSHID → USB HID → OS
```

## Hardware Configuration

### UPS Hardware Setup
- **I2C Address**: 0x55 (defined in `ups.h`)
- **Product ID**: 0x42AA for 3-cell UPS (defined in `ups.h`)
- **Status LED**: Pin 13 (defined in `ups.h`)

### Battery Specifications
- **Cell Configuration**: 3 cells in series
- **Voltage Range**: 9.2V - 12.6V (3 × 3.6V nominal)
- **Capacity**: 4000mAh per cell (12Ah total)
- **Cell Voltage Range**: 2.6V - 4.2V per cell

## Software Configuration

### Feature Flags (in `config.h`)
```cpp
// Enable UPS functionality
#define ENABLE_HID_POWER_DEVICE 1

// Debug output (uncomment to enable)
//#define DEBUG_PRINT_UPS 1
```

### HID Configuration (in `hid_config.h`)
```cpp
// Power Device Report ID
#define HID_POWER_DEVICE_REPORT_ID 1
```

## UPS Control Module

### UPSControl Class

The `UPSControl` class manages all UPS functionality:

#### Key Methods
- `begin()` - Initialize UPS hardware and communication
- `update()` - Main update loop (called from main loop)
- `getStatus()` - Get current UPS status
- `reportBatteryStatus()` - Send HID report to OS

#### Status Structure
```cpp
struct UPSStatus {
    uint16_t voltage_mV;           // Battery voltage in millivolts
    uint16_t current_mA;           // Battery current in milliamps
    uint16_t capacity_percent;     // Battery capacity percentage (0-100)
    uint16_t temperature_celsius;  // Battery temperature in Celsius
    bool is_charging;              // True if battery is charging
    bool is_connected;             // True if UPS is connected and responding
    uint32_t last_update_ms;       // Timestamp of last successful update
};
```

### Update Timing
- **UPS Data Reading**: Every 1 second
- **Status LED Update**: Every 500ms
- **HID Reporting**: Every 5 seconds
- **All operations are non-blocking**

## HID Power Device Reporting

### Report Structure
```cpp
struct HIDPowerDeviceReport {
    uint8_t report_id;           // Report ID (1)
    uint8_t battery_present;     // 0 = not present, 1 = present
    uint8_t battery_capacity;    // Battery capacity percentage (0-100)
    uint16_t battery_voltage;    // Battery voltage in millivolts
    int16_t battery_current;     // Battery current in milliamps
    uint8_t battery_temperature; // Battery temperature in Celsius
    uint8_t battery_status;      // Battery status (charging, discharging, etc.)
    uint16_t runtime_to_empty;   // Estimated runtime to empty in minutes
};
```

### Battery Status Values
- `BATTERY_STATUS_UNKNOWN` (0x00) - Status unknown
- `BATTERY_STATUS_FULLY_CHARGED` (0x01) - Battery fully charged
- `BATTERY_STATUS_DISCHARGING` (0x02) - Battery discharging
- `BATTERY_STATUS_CHARGING` (0x03) - Battery charging
- `BATTERY_STATUS_CRITICAL` (0x04) - Battery critically low

## Status LED Behavior

The status LED (Pin 13) provides visual feedback:

- **Solid ON**: Battery is charging
- **Slow Blink**: Battery > 50% capacity
- **Medium Blink**: Battery 20-50% capacity
- **Fast Blink**: Battery < 20% capacity
- **Very Fast Blink**: UPS disconnected/communication error

## Integration with Main Loop

### Setup Phase
```cpp
void setup() {
    // ... other initialization ...
    
    #if ENABLE_HID_POWER_DEVICE
    if (setupUPS()) {
        Serial.println("UPS setup completed");
    } else {
        Serial.println("UPS setup failed - continuing without UPS");
    }
    #endif
}
```

### Main Loop
```cpp
void loop() {
    loopGamepad();
    
    // Update UPS functionality (non-blocking)
    #if ENABLE_HID_POWER_DEVICE
    loopUPS();
    #endif
}
```

## Battery Capacity Calculation

The system uses a sophisticated voltage-to-State-of-Charge (SoC) calculation:

### Voltage-to-SoC Mapping
- Uses lookup tables for different discharge rates (0.8A and 2A)
- Compensates for internal resistance voltage drop
- Linear interpolation between voltage points
- Accounts for charging vs. discharging states

### SoC Calculation Process
1. Read raw voltage from UPS
2. Compensate for internal resistance voltage drop
3. Select appropriate voltage-to-SoC lookup table
4. Interpolate between voltage points
5. Return capacity percentage (0-100%)

## Error Handling

### Communication Errors
- Automatic retry on I2C communication failures
- Graceful degradation when UPS is disconnected
- Debug output for troubleshooting

### Invalid Data Handling
- Clamps capacity values to 0-100% range
- Handles missing or corrupted data gracefully
- Continues operation even with partial data

## Debug Output

Enable debug output by uncommenting in `config.h`:
```cpp
#define DEBUG_PRINT_UPS 1
```

### Debug Information
- UPS initialization status
- Real-time battery readings
- HID report contents
- Communication errors
- Status LED state changes

## Operating System Integration

### Windows
- Battery status appears in system tray
- Power settings show battery information
- Low battery warnings work normally
- Battery percentage updates in real-time

### Linux
- Battery information available via `/sys/class/power_supply/`
- Desktop environments display battery status
- Power management tools recognize the battery
- Suspend/hibernate on low battery works

## Troubleshooting

### Common Issues

1. **UPS Not Detected**
   - Check I2C wiring (SDA/SCL)
   - Verify UPS power supply
   - Check I2C address (should be 0x55)

2. **Battery Status Not Showing in OS**
   - Verify HID Power Device report ID configuration
   - Check USB connection
   - Ensure HID library is properly configured

3. **Inaccurate Battery Percentage**
   - Calibrate voltage-to-SoC lookup tables
   - Check for voltage measurement errors
   - Verify battery specifications

### Debug Steps
1. Enable debug output (`DEBUG_PRINT_UPS 1`)
2. Check serial monitor for UPS communication
3. Verify I2C communication with UPS
4. Test HID report generation
5. Check OS device manager for HID device

## Future Enhancements

### Potential Improvements
1. **Battery Calibration**: User-configurable voltage-to-SoC curves
2. **Power Management**: Automatic system shutdown on low battery
3. **Charging Control**: Intelligent charging current management
4. **Temperature Monitoring**: Thermal protection and monitoring
5. **Battery Health**: Cycle count and health estimation

### Configuration Options
1. **Report Intervals**: Configurable HID reporting frequency
2. **LED Patterns**: Customizable status LED behavior
3. **Thresholds**: Adjustable low battery and critical levels
4. **Calibration**: User-adjustable voltage-to-SoC mapping

This implementation provides a robust foundation for UPS functionality while maintaining the existing gamepad capabilities of the LatteDeck project.
