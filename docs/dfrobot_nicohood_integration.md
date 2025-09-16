# DFRobot LPUPS and NicoHood HID Integration

## Overview

This document explains how the LatteDeck project properly integrates the DFRobot LPUPS library for battery reporting with the NicoHood HID library for mouse and keyboard functionality.

## Problem Solved

The original implementation had issues with Windows battery recognition because it was using a custom HID implementation instead of proper HID Power Device reporting. The solution is to use:

- **DFRobot LPUPS Library**: Handles UPS I2C communication only
- **Custom UPS HID Implementation**: Provides HID Power Device reporting using NicoHood HID library
- **NicoHood HID Library**: Handles mouse and keyboard functionality

## Integration Approach

### 1. Library Configuration

```cpp
// Disable DFRobot library's HID functionality to avoid conflicts with NicoHood HID
#ifndef UPS_HID_NICOHOOD
#define UPS_HID_NICOHOOD 0
#endif

// Configure Report IDs for composite HID device
#include "hid_config.h"  // Must be included before HID-Project.h
```

The `hid_config.h` file configures Report IDs for all HID interfaces:
- UPS Power Device: Report ID 1
- Mouse: Report ID 2  
- Keyboard: Report ID 3

### 2. Initialization Order

```cpp
void setup() {
    // Initialize NicoHood HID for mouse and keyboard functionality
    Mouse.begin();
    Keyboard.begin();
    
    // Initialize custom UPS HID for battery reporting
    UPSHID::begin();
    
    // Initialize UPS with DFRobot library (I2C communication only)
    setupUPS();
}
```

### 3. UPS HID Reporting

Custom UPS HID implementation provides proper battery reporting:

```cpp
// Use custom UPS HID reporting for proper Windows compatibility
UPSHID::sendBatteryReport(iRemaining, iRunTimeToEmpty, iPresentStatus);
```

### 4. Mouse and Keyboard Control

The NicoHood library provides direct mouse and keyboard control:

```cpp
// Mouse functions
Mouse.move(x, y);
Mouse.press(MOUSE_LEFT);
Mouse.release(MOUSE_LEFT);

// Keyboard functions
Keyboard.press(KEY_W);
Keyboard.release(KEY_W);
```

## Key Benefits

### 1. **Proven Windows Compatibility**
- Custom UPS HID implementation follows USB HID Power Device specification
- Proper Report ID configuration for all HID interfaces (UPS: ID 1, Mouse: ID 2, Keyboard: ID 3)
- Eliminates "Default report ID is only allowed for devices with one top level collection" errors
- No additional drivers required - uses standard Windows HID drivers

### 2. **Simplified Code**
- Removed custom CompositeHID wrapper
- Direct use of library functions
- Less code to maintain

### 3. **Better Reliability**
- DFRobot library handles UPS I2C communication
- Custom UPS HID works with NicoHood HID without conflicts
- Proper error handling

### 4. **Standards Compliance**
- Custom UPS HID follows USB HID Power Device specifications
- NicoHood library follows USB HID specifications for mouse/keyboard

## File Changes

### Modified Files

1. **`ups_utils.h`**:
   - Disabled DFRobot HID functionality (`UPS_HID_NICOHOOD 0`)
   - Added custom UPS HID approach

2. **`ups_utils.cpp`**:
   - Updated `initPowerDevice()` to use custom UPS HID

3. **`ups_ctrl.cpp`**:
   - Updated battery reporting to use custom UPS HID functions

4. **`gamepad.cpp`**:
   - Direct use of NicoHood Mouse and Keyboard functions
   - Removed CompositeHID wrapper calls

5. **`gamepad.h`**:
   - Removed CompositeHID include

6. **`latte-deck.ino`**:
   - Direct initialization of NicoHood HID and UPS HID
   - Removed CompositeHID dependency

### Added Files

- **`ups_hid.h`** and **`ups_hid.cpp`**: Custom UPS HID implementation

### Removed Files

- **`composite_hid.cpp`** and **`composite_hid.h`**: No longer needed

## Testing

### Battery Reporting
1. **Device Manager**: Should show battery device without errors
2. **Power Settings**: Battery percentage should display correctly
3. **System Tray**: Battery icon should show accurate status

### Mouse and Keyboard
1. **Mouse Movement**: Right joystick should move cursor smoothly
2. **Mouse Clicks**: Joystick button should register as left click
3. **Keyboard Input**: Left joystick should type WASD keys
4. **Multi-key Support**: Should support diagonal movement (W+A)

## Troubleshooting

### Battery Not Showing
1. Check UPS I2C connection
2. Verify PID value in `config.h`
3. Check serial output for UPS initialization messages

### Mouse/Keyboard Not Working
1. Check gamepad enable pin (pin 4)
2. Verify joystick wiring
3. Check debug output for input readings

### Library Conflicts
1. Ensure both libraries are properly installed
2. Check include order in source files
3. Verify no duplicate HID definitions

## Migration Notes

### Breaking Changes
- Removed CompositeHID wrapper functions
- Direct use of library functions required
- Updated initialization sequence

### Backward Compatibility
- All gamepad functionality remains the same
- Same pin assignments and controls
- Same configuration options

## Conclusion

This integration approach provides the best of both worlds:
- **Reliable battery reporting** using the proven DFRobot library
- **Robust mouse/keyboard functionality** using the NicoHood library
- **Simplified codebase** with fewer custom implementations
- **Better Windows compatibility** with proper HID specifications

The result is a more reliable, maintainable, and standards-compliant implementation that properly reports battery status to Windows while providing full mouse and keyboard functionality.
