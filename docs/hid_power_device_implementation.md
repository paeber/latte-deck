# HID Power Device Implementation for UPS Battery Reporting

## Overview

This document describes the implementation of HID Power Device reporting for UPS battery status in the LatteDeck project. The implementation allows the operating system to receive battery status information through USB HID reports without interfering with existing keyboard and mouse functionality.

## Architecture

### Components

1. **HID Descriptor** (`hid_descriptor.h`)
   - Defines the composite HID device descriptor
   - Includes Power Device, Mouse, and Keyboard interfaces
   - Uses Report IDs to distinguish between different device types

2. **HID Configuration** (`hid_config.h`)
   - Configures Report IDs for all HID interfaces
   - Enables composite device support
   - Ensures Windows compatibility

3. **UPS HID Interface** (`ups_hid.h`, `ups_hid.cpp`)
   - Main interface for UPS battery status reporting
   - Converts UPS status data to HID Power Device reports
   - Manages report timing and data conversion

4. **Leonardo HID Implementation** (`hid_leonardo.h`, `hid_leonardo.cpp`)
   - Arduino Leonardo-specific HID implementation
   - Handles USB HID report sending
   - Provides debug output and error handling

## HID Report Structure

### Power Device Report (Report ID 1)

```c
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
```

### Battery Status Values

- `BATTERY_STATUS_UNKNOWN` (0x00): Battery status unknown
- `BATTERY_STATUS_FULLY_CHARGED` (0x01): Battery fully charged
- `BATTERY_STATUS_DISCHARGING` (0x02): Battery discharging
- `BATTERY_STATUS_CHARGING` (0x03): Battery charging
- `BATTERY_STATUS_CRITICAL` (0x04): Battery critical level

## Report IDs

The implementation uses the following Report IDs to avoid conflicts:

- **Report ID 1**: Power Device (UPS Battery Status)
- **Report ID 2**: Mouse
- **Report ID 3**: Keyboard

## Usage

### Initialization

```cpp
// In setup()
if (setupUPS()) {
    if (setupUPSHID()) {
        Serial.println("UPS HID reporting initialized");
    }
}
```

### Reporting Battery Status

```cpp
// In loop()
reportUPSBatteryStatus();
```

The system automatically:
1. Reads UPS status from the UPS control module
2. Converts the status to HID Power Device report format
3. Sends the report via USB HID every 5 seconds
4. Provides debug output when `DEBUG_PRINT_UPS` is enabled

## Implementation Notes

### Current Limitations

1. **USB Descriptor**: The current implementation uses a workaround approach because the HID-Project library doesn't natively support Power Device reports. A full implementation would require modifying the USB descriptor.

2. **Report Sending**: The actual HID report sending is currently simulated. In a production implementation, you would need to:
   - Modify the USB descriptor to include the Power Device interface
   - Use the appropriate USB HID functions to send reports
   - Handle USB communication errors

### Future Improvements

1. **USB Descriptor Modification**: Implement a custom USB descriptor that includes the Power Device interface alongside Mouse and Keyboard.

2. **Native HID Support**: Extend the HID-Project library or create a custom HID implementation that supports Power Device reports.

3. **Error Handling**: Add proper error handling for USB communication failures.

4. **Configuration**: Make the report interval and other parameters configurable.

## Debug Output

When `DEBUG_PRINT_UPS` is enabled, the system provides detailed debug output including:

- HID report data in hexadecimal format
- Battery status information
- Report timing information
- Error messages

## Compatibility

- **Windows**: Should work with Windows 10/11 when properly implemented
- **Linux**: Should work with Linux systems that support HID Power Device
- **macOS**: Compatibility depends on macOS HID Power Device support

## References

- [HID Usage Tables for USB Power Device Class](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
- [DFRobot LPUPS Library](https://github.com/DFRobot/DFRobot_LPUPS)
- [HID-Project Library](https://github.com/NicoHood/HID)
