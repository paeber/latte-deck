# Arduino Library Modifications for HID Power Device Support

## Overview

This document provides detailed instructions for modifying Arduino libraries to enable full HID Power Device support for UPS battery reporting. The modifications allow the Arduino Leonardo to send proper Power Device HID reports to the operating system.

## Table of Contents

1. [Current Implementation Status](#current-implementation-status)
2. [Required Modifications](#required-modifications)
3. [Step-by-Step Instructions](#step-by-step-instructions)
4. [Alternative Approaches](#alternative-approaches)
5. [Testing and Validation](#testing-and-validation)
6. [Troubleshooting](#troubleshooting)

## Current Implementation Status

### Working Components

✅ **HID Report Structure**: Complete Power Device report structure with all required fields
✅ **Report Generation**: Converts UPS status to HID Power Device reports
✅ **Debug Output**: Comprehensive debug information and report printing
✅ **Integration**: Seamless integration with existing UPS and gamepad functionality
✅ **Workaround Implementation**: Uses Consumer Control as a proxy for battery status

### Limitations

❌ **Direct HID Report Sending**: Cannot send native Power Device HID reports
❌ **USB Descriptor**: Current descriptor doesn't include Power Device interface
❌ **OS Recognition**: Operating system doesn't see device as a UPS/Power Device

## Required Modifications

To enable full HID Power Device support, you need to modify the following Arduino components:

### 1. USB Descriptor Modification
- **File**: `cores/arduino/USBDesc.h`
- **Purpose**: Add Power Device interface to USB descriptor
- **Impact**: Allows OS to recognize device as a composite HID device with Power Device capability

### 2. HID Report Descriptor
- **File**: `cores/arduino/HID.cpp`
- **Purpose**: Include Power Device HID report descriptor
- **Impact**: Defines the structure and format of Power Device reports

### 3. USB HID Interface
- **File**: `cores/arduino/HID.cpp`
- **Purpose**: Add Power Device report sending functionality
- **Impact**: Enables actual sending of Power Device HID reports

## Step-by-Step Instructions

### Method 1: Arduino Core Modification (Recommended)

#### Step 1: Backup Arduino Core Files

```bash
# Navigate to Arduino installation directory
cd /path/to/arduino/hardware/arduino/avr/cores/arduino/

# Create backup directory
mkdir backup

# Backup original files
cp USBCore.cpp backup/
cp USBDesc.h backup/
cp HID.cpp backup/
cp HID.h backup/
```

#### Step 2: Modify USB Descriptor (`USBDesc.h`)

Add the following to the USB configuration descriptor:

```c
// Add after existing HID interfaces
// Power Device Interface Descriptor
0x09,       // bLength
0x04,       // bDescriptorType (Interface)
0x02,       // bInterfaceNumber (2) - adjust based on existing interfaces
0x00,       // bAlternateSetting
0x01,       // bNumEndpoints (1)
0x03,       // bInterfaceClass (HID)
0x00,       // bInterfaceSubClass
0x00,       // bInterfaceProtocol
0x00,       // iInterface (String Index)

// HID Descriptor for Power Device
0x09,       // bLength
0x21,       // bDescriptorType (HID)
0x11, 0x01, // bcdHID (1.11)
0x00,       // bCountryCode
0x01,       // bNumDescriptors
0x22,       // bDescriptorType (Report)
LOW(sizeof(PowerDeviceReportDescriptor)), HIGH(sizeof(PowerDeviceReportDescriptor)),

// Endpoint Descriptor for Power Device
0x07,       // bLength
0x05,       // bDescriptorType (Endpoint)
0x84,       // bEndpointAddress (IN, 4) - adjust based on available endpoints
0x03,       // bmAttributes (Interrupt)
0x40, 0x00, // wMaxPacketSize (64)
0x0A,       // bInterval (10ms)
```

#### Step 3: Add Power Device HID Report Descriptor (`HID.cpp`)

Add the following report descriptor:

```c
// Power Device HID Report Descriptor
const u8 PowerDeviceReportDescriptor[] PROGMEM = {
    0x05, 0x84,        // Usage Page (Power Device)
    0x09, 0x04,        // Usage (UPS)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    
    // Battery Present
    0x05, 0x85,        //   Usage Page (Battery System)
    0x09, 0x66,        //   Usage (Battery Present)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Capacity (0-100%)
    0x09, 0x66,        //   Usage (Battery Capacity)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x64, 0x00,  //   Logical Maximum (100)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Voltage (0-65535 mV)
    0x09, 0x30,        //   Usage (Battery Voltage)
    0x15, 0x00,        //   Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65535)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Current (-32768 to 32767 mA)
    0x09, 0x31,        //   Usage (Battery Current)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Temperature (0-255°C)
    0x09, 0x32,        //   Usage (Battery Temperature)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Battery Status
    0x09, 0x33,        //   Usage (Battery Status)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x04,        //   Logical Maximum (4)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Runtime to Empty (0-65535 minutes)
    0x09, 0x34,        //   Usage (Runtime to Empty)
    0x15, 0x00,        //   Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65535)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data, Variable, Absolute)
    
    // Padding to align to byte boundary
    0x75, 0x07,        //   Report Size (7)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Constant, Variable, Absolute)
    
    0xC0,              // End Collection
};
```

#### Step 4: Add Power Device HID Functions (`HID.cpp`)

Add the following functions to the HID class:

```c
// Power Device HID endpoint
#define POWER_DEVICE_ENDPOINT 4  // Adjust based on available endpoints

// Send Power Device HID report
int HID_::SendPowerDeviceReport(const void* data, int len) {
    if (len != 9) return -1; // Power Device report must be 9 bytes
    
    return USB_Send(POWER_DEVICE_ENDPOINT | TRANSFER_PGM, data, len);
}

// Power Device report structure
struct PowerDeviceReport {
    uint8_t report_id;           // Report ID (1)
    uint8_t battery_present;     // 0 = not present, 1 = present
    uint8_t battery_capacity;    // Battery capacity percentage (0-100)
    uint16_t battery_voltage;    // Battery voltage in millivolts
    int16_t battery_current;     // Battery current in milliamps
    uint8_t battery_temperature; // Battery temperature in Celsius
    uint8_t battery_status;      // Battery status
    uint16_t runtime_to_empty;   // Runtime to empty in minutes
} __attribute__((packed));
```

#### Step 5: Update HID Header File (`HID.h`)

Add the following to the HID class declaration:

```c
class HID_ : public PluggableUSBModule {
    // ... existing members ...
    
public:
    // ... existing methods ...
    
    // Power Device methods
    int SendPowerDeviceReport(const void* data, int len);
    
    // ... existing methods ...
};
```

### Method 2: Custom USB Library (Alternative)

If you prefer not to modify the Arduino core, you can create a custom USB library:

#### Step 1: Create Custom USB Library

Create a new library in `libraries/CustomHID/`:

```
libraries/
└── CustomHID/
    ├── src/
    │   ├── CustomHID.h
    │   └── CustomHID.cpp
    └── library.properties
```

#### Step 2: Implement Custom USB HID Interface

**CustomHID.h**:
```c
#ifndef CUSTOM_HID_H
#define CUSTOM_HID_H

#include <Arduino.h>
#include <PluggableUSB.h>

class CustomHID : public PluggableUSBModule {
private:
    uint8_t epType[1];
    
public:
    CustomHID();
    
    // PluggableUSBModule implementation
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    
    // Power Device methods
    int sendPowerDeviceReport(const void* data, int len);
};

extern CustomHID PowerDevice;

#endif
```

**CustomHID.cpp**:
```c
#include "CustomHID.h"

// Implementation of custom USB HID interface
// This would include the USB descriptor and report sending logic
```

### Method 3: Modify HID-Project Library

You can also modify the HID-Project library to add Power Device support:

#### Step 1: Locate HID-Project Library

Find the HID-Project library in your Arduino libraries folder:
```
libraries/HID-Project/src/
```

#### Step 2: Add Power Device Support

Modify the following files:
- `HID-Project.h`: Add Power Device class declaration
- `HID-APIs/PowerDeviceAPI.h`: Create Power Device API
- `HID-APIs/PowerDeviceAPI.cpp`: Implement Power Device functionality

## Alternative Approaches

### Approach 1: Consumer Control Workaround (Current Implementation)

**Pros**:
- Works with existing libraries
- No modifications required
- Immediate implementation

**Cons**:
- Not a true Power Device interface
- Limited functionality
- OS may not recognize as UPS

### Approach 2: System Control Integration

Similar to Consumer Control but uses System Control HID interface:

```c
#include <HID-Project.h>

// Send battery status via System Control
void sendBatteryStatusViaSystemControl(uint8_t capacity, uint8_t status) {
    if (status == BATTERY_STATUS_CRITICAL) {
        System.write(SYSTEM_POWER_DOWN);
    }
    // Additional system control codes...
}
```

### Approach 3: Serial Communication

Use serial communication to send battery status to a host application:

```c
void sendBatteryStatusSerial(const UPSStatus& status) {
    Serial.print("BATTERY:");
    Serial.print(status.capacity_percent);
    Serial.print(",");
    Serial.print(status.voltage_mV);
    Serial.print(",");
    Serial.print(status.is_charging ? "CHARGING" : "DISCHARGING");
    Serial.println();
}
```

## Testing and Validation

### Step 1: Verify USB Descriptor

Use USB descriptor analysis tools to verify the modified descriptor:

**Windows**: USBView, USB Device Tree Viewer
**Linux**: `lsusb -v`, `usbhid-dump`
**macOS**: USB Prober, System Information

### Step 2: Test HID Report Sending

Monitor HID reports using:

**Windows**: 
- Device Manager → Human Interface Devices
- Windows SDK tools

**Linux**:
```bash
# Monitor HID reports
sudo usbhid-dump -d vid:pid

# Check power supply status
cat /sys/class/power_supply/*/uevent
```

**macOS**:
- Console app for system logs
- IORegistryExplorer

### Step 3: Validate OS Recognition

Check if the operating system recognizes the device as a UPS:

**Windows**:
- Control Panel → Power Options
- Device Manager → Batteries

**Linux**:
```bash
# Check UPS detection
upsc -l
upower -i /org/freedesktop/UPower/devices/ups_*
```

## Troubleshooting

### Common Issues

#### Issue 1: USB Device Not Recognized

**Symptoms**: Device not appearing in Device Manager
**Solutions**:
- Check USB descriptor syntax
- Verify endpoint addresses don't conflict
- Ensure total configuration length is correct

#### Issue 2: HID Reports Not Received

**Symptoms**: Device recognized but no battery status updates
**Solutions**:
- Verify HID report descriptor matches report structure
- Check report ID consistency
- Ensure endpoint is configured for interrupt transfers

#### Issue 3: Compilation Errors

**Symptoms**: Arduino IDE compilation fails
**Solutions**:
- Check include paths
- Verify all required files are present
- Ensure compatibility with Arduino core version

### Debug Techniques

#### Enable USB Debug Output

Add debug output to USB functions:

```c
#ifdef DEBUG_USB
#define USB_DEBUG(x) Serial.print(x)
#else
#define USB_DEBUG(x)
#endif
```

#### Monitor USB Traffic

Use USB analyzers or software tools:
- Wireshark with USBPcap (Windows)
- usbmon (Linux)
- USB Prober (macOS)

## Conclusion

Implementing full HID Power Device support requires modifying Arduino libraries, but the result is a proper UPS interface that operating systems can recognize and use for power management. The current workaround implementation provides a foundation that can be enhanced with these modifications.

For immediate use, the Consumer Control workaround provides basic functionality, while the library modifications enable full Power Device compliance and OS integration.
