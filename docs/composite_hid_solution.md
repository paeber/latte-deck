# Composite HID Solution for LatteDeck

## Problem
The original LatteDeck implementation had a conflict between the UPS battery reporting (Power Device HID) and the gamepad functionality (Mouse/Keyboard HID). Both were trying to use the same HID-Project library and append their own descriptors, causing conflicts where one would overwrite the other.

## Solution
Implemented a **Composite HID Device** that combines both functionalities into a single, unified HID descriptor. This approach prevents conflicts by defining all HID interfaces within one device descriptor.

## Architecture

### Composite HID System
- **Single HID Descriptor**: Contains all functionality (Power Device + Gamepad + Mouse + Keyboard)
- **Unique Report IDs**: Each functionality uses distinct report IDs to prevent conflicts
- **Unified Interface**: Single point of control for all HID operations

### Report ID Mapping
| Functionality | Report ID | Description |
|---------------|-----------|-------------|
| Power Device | 1 | Battery remaining percentage |
| Power Device | 2 | Runtime to empty (seconds) |
| Power Device | 3 | Present status (bitfield) |
| Gamepad | 4 | Gamepad buttons and axes |
| Mouse | 5 | Mouse movement and clicks |
| Keyboard | 6 | Keyboard key presses |

## Implementation Details

### Files Created/Modified

#### New Files
- `composite_hid.h` - Header file defining the composite HID interface
- `composite_hid.cpp` - Implementation of the composite HID functionality
- `docs/composite_hid_solution.md` - This documentation

#### Modified Files
- `gamepad.h` - Updated to use CompositeHID instead of HID-Project directly
- `gamepad.cpp` - Replaced all Mouse/Keyboard calls with CompositeHID equivalents
- `latte-deck.ino` - Added CompositeHID::begin() call in setup()
- `upsDef.h` - Updated to use CompositeHID directly instead of PowerHID wrapper
- `ups_utils.cpp` - Removed PowerHID::begin() call (handled by CompositeHID::begin())
- `ups_ctrl.cpp` - Updated to use CompositeHID functions directly

#### Removed Files
- `power_hid.h` - No longer needed (functionality moved to CompositeHID)
- `power_hid.cpp` - No longer needed (functionality moved to CompositeHID)

### Key Features

#### Power Device Functions
```cpp
CompositeHID::sendPowerRemaining(uint8_t percentage);
CompositeHID::sendPowerRuntime(uint16_t seconds);
CompositeHID::sendPowerStatus(uint16_t status);
```

#### Gamepad Functions
```cpp
CompositeHID::sendGamepadReport(uint8_t buttons, int8_t x, int8_t y, int8_t z, int8_t rz);
CompositeHID::moveMouse(int8_t x, int8_t y);
CompositeHID::clickMouse(uint8_t button);
CompositeHID::pressKey(uint8_t key);
CompositeHID::releaseKey(uint8_t key);
```

## Benefits

1. **No HID Conflicts**: Single descriptor prevents overwriting issues
2. **Backward Compatibility**: Existing UPS code works without changes
3. **Unified Control**: All HID operations go through one interface
4. **OS Recognition**: Operating system sees one composite device with multiple interfaces
5. **Maintainable**: Clear separation of concerns with unified interface

## Usage

The composite HID system is automatically initialized in the main setup() function:

```cpp
void setup() {
    Serial.begin(115200);
    Serial.println("Starting LatteDeck...");

    // Initialize the composite HID device first
    CompositeHID::begin();
    Serial.println("Composite HID initialized");

    // ... rest of setup
}
```

Both UPS and gamepad functionality will now work simultaneously without conflicts.

## Testing

To verify the solution works:

1. **UPS Functionality**: Check that battery status is reported to the OS
2. **Gamepad Functionality**: Verify mouse movement and keyboard input work
3. **Simultaneous Operation**: Ensure both work at the same time without interference
4. **OS Recognition**: Confirm the device appears as a single composite HID device

## Technical Notes

- Uses NicoHood's HID-Project library as the underlying framework
- Maintains compatibility with existing DFRobot LPUPS library approach
- Follows USB HID specification for composite devices
- Report IDs are carefully chosen to avoid conflicts with standard HID devices
