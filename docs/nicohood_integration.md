# NicoHood HID Library Integration

## Overview

The LatteDeck project now uses the [NicoHood HID library](https://github.com/NicoHood/HID/tree/master) for proper keyboard and mouse functionality, while maintaining custom Power Device reporting for UPS battery status.

## Why NicoHood Integration?

### Problems with Previous Implementation
1. **Broken Key Release Logic** - `releaseKey()` released ALL keys instead of specific keys
2. **No Key State Tracking** - Couldn't track which keys were pressed
3. **No Multi-Key Support** - Couldn't hold multiple keys simultaneously
4. **Custom HID Descriptors** - Reinventing the wheel instead of using proven library

### Benefits of NicoHood Integration
1. **Proper Key Management** - Correct press/release behavior
2. **Multi-Key Support** - Can hold multiple keys (W+A for diagonal movement)
3. **Well-Tested Library** - 2.5k stars, actively maintained
4. **Standard Compliance** - Follows USB HID specifications
5. **Simplified Code** - Less custom code to maintain

## Implementation Details

### Library Structure
```cpp
#include <HID-Project.h>  // Main library
#include <HID-Settings.h> // Configuration
```

### Device Initialization
```cpp
void CompositeHID::begin()
{
  // Initialize NicoHood HID devices
  Mouse.begin();
  Keyboard.begin();
  
  // Register Power Device descriptor for UPS functionality
  HID().AppendDescriptor(&powerDeviceNode);
  
  // Send initial Power Device report
  sendPowerReport(100, 3600, 0x0080);
}
```

### Mouse Functions (NicoHood API)
```cpp
void CompositeHID::moveMouse(int8_t x, int8_t y)
{
  Mouse.move(x, y);
}

void CompositeHID::pressMouse(uint8_t button)
{
  Mouse.press(button);
}

void CompositeHID::releaseMouse(uint8_t button)
{
  Mouse.release(button);
}

void CompositeHID::clickMouse(uint8_t button)
{
  Mouse.click(button);
}
```

### Keyboard Functions (NicoHood API)
```cpp
void CompositeHID::pressKey(uint8_t key)
{
  Keyboard.press(key);
}

void CompositeHID::releaseKey(uint8_t key)
{
  Keyboard.release(key);
}

void CompositeHID::releaseAllKeys()
{
  Keyboard.releaseAll();
}

void CompositeHID::pressAndReleaseKey(uint8_t key)
{
  Keyboard.press(key);
  delay(10);
  Keyboard.release(key);
}
```

### Power Device (Custom Implementation)
```cpp
int CompositeHID::sendPowerReport(uint8_t percentage, uint16_t runtime, uint16_t status)
{
  // Custom implementation for UPS battery reporting
  uint8_t report[6];
  report[0] = LATTE_REPORT_ID_POWER_DEVICE;
  report[1] = percentage;
  report[2] = runtime & 0xFF;
  report[3] = (runtime >> 8) & 0xFF;
  report[4] = status & 0xFF;
  report[5] = (status >> 8) & 0xFF;
  
  return HID().SendReport(LATTE_REPORT_ID_POWER_DEVICE, report, sizeof(report));
}
```

## Key Improvements

### 1. **Proper Key Release**
**Before (Broken):**
```cpp
void CompositeHID::releaseKey(uint8_t key)
{
  sendKeyboardReport(0, 0, 0, 0, 0, 0, 0);  // Released ALL keys!
}
```

**After (Correct):**
```cpp
void CompositeHID::releaseKey(uint8_t key)
{
  Keyboard.release(key);  // Releases only the specified key
}
```

### 2. **Multi-Key Support**
**Before:** Could only press one key at a time
**After:** Can hold multiple keys simultaneously (W+A for diagonal movement)

### 3. **Simplified Code**
**Before:** 200+ lines of custom HID implementation
**After:** ~50 lines using proven library functions

### 4. **Better Mouse Handling**
**Before:** Custom mouse report generation
**After:** Standard NicoHood Mouse API with proper button handling

## Gamepad Integration

The gamepad code now works correctly with proper key management:

```cpp
// Press W key for up movement
CompositeHID::pressKey(ACTION_JOYSTICK_L_UP);

// Release down key while keeping up key pressed
CompositeHID::releaseKey(ACTION_JOYSTICK_L_DOWN);

// Press A key for left movement (now W+A works together)
CompositeHID::pressKey(ACTION_JOYSTICK_L_LEFT);
```

## Library Requirements

### Installation
1. **Arduino IDE Library Manager:**
   - Sketch → Include Library → Manage Libraries
   - Search for "HID-Project"
   - Install by NicoHood

2. **Manual Installation:**
   - Download from [GitHub](https://github.com/NicoHood/HID)
   - Extract to Arduino libraries folder
   - Restart Arduino IDE

### Board Compatibility
- **Arduino Leonardo/Micro:** Native HID support
- **Arduino Uno/Mega:** Requires HoodLoader2 firmware
- **Other 32u4 boards:** Native HID support

## Testing

### Keyboard Testing
1. **Single Key Press:** Press and release individual keys
2. **Multi-Key Press:** Hold W+A for diagonal movement
3. **Key Release:** Verify only specified keys are released

### Mouse Testing
1. **Movement:** Joystick should move cursor smoothly
2. **Button Press:** Joystick button should press/release correctly
3. **Multi-Button:** Support for left/right mouse buttons

### UPS Testing
1. **Battery Status:** Windows should show battery percentage
2. **Charging Status:** AC adapter detection
3. **Runtime:** Time remaining estimation

## Migration Notes

### Breaking Changes
- Removed custom `sendMouseReport()` and `sendKeyboardReport()` functions
- Mouse button constants changed from `0x01` to `MOUSE_LEFT`
- Simplified API with fewer functions

### Backward Compatibility
- All existing gamepad code continues to work
- Same function names maintained
- Improved behavior with no code changes required

## Benefits Summary

1. **Reliability** - Well-tested library with 2.5k stars
2. **Maintainability** - Less custom code to maintain
3. **Standards Compliance** - Follows USB HID specifications
4. **Feature Rich** - Multi-key support, proper state management
5. **Community Support** - Active development and community
6. **Documentation** - Extensive examples and documentation

The integration with the [NicoHood HID library](https://github.com/NicoHood/HID/tree/master) provides a robust, reliable foundation for HID functionality while maintaining the custom UPS Power Device reporting required for battery status.
