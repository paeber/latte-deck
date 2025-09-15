# Composite HID Solution for LatteDeck

## Problem
The original LatteDeck implementation had multiple HID conflicts:

1. **UPS vs Gamepad Conflict**: Both UPS battery reporting (Power Device HID) and gamepad functionality (Mouse/Keyboard HID) were trying to use the same HID-Project library and append their own descriptors, causing conflicts where one would overwrite the other.

2. **DFRobot vs NicoHood HID Conflict**: The [DFRobot LPUPS library](https://github.com/DFRobot/DFRobot_LPUPS) includes its own HID implementation (based on HIDPowerDevice library), which conflicts with the [NicoHood HID-Project library](https://github.com/NicoHood/HID) that we want to use for enhanced functionality.

## Solution
Implemented a **Composite HID Device** that combines both functionalities into a single, unified HID descriptor. This approach prevents conflicts by:

1. **Prioritizing NicoHood HID Library**: Ensures the [NicoHood HID-Project library](https://github.com/NicoHood/HID) takes precedence over the DFRobot library's HID implementation
2. **Disabling DFRobot HID**: Sets `UPS_HID_NICOHOOD = 0` to disable the DFRobot library's HID functionality
3. **Single Unified Descriptor**: Defines all HID interfaces within one device descriptor to prevent conflicts
4. **Include Order Control**: Ensures NicoHood HID headers are included before any DFRobot library headers

## Architecture

### Composite HID System
- **Single HID Descriptor**: Contains all functionality (Power Device + Mouse + Keyboard)
- **Unique Report IDs**: Each functionality uses distinct report IDs to prevent conflicts
- **Unified Interface**: Single point of control for all HID operations
- **Windows-Compatible**: Uses standard Mouse and Keyboard HID devices instead of Gamepad for better Windows recognition

### Report ID Mapping
| Functionality | Report ID | Description |
|---------------|-----------|-------------|
| Power Device | 1 | Battery remaining percentage |
| Power Device | 2 | Runtime to empty (seconds) |
| Power Device | 3 | Present status (bitfield) |
| Mouse | 4 | Mouse movement, clicks, and scroll wheel |
| Keyboard | 5 | Keyboard key presses |

## Implementation Details

### Files Created/Modified

#### New Files
- `composite_hid.h` - Header file defining the composite HID interface
- `composite_hid.cpp` - Implementation of the composite HID functionality
- `docs/composite_hid_solution.md` - This documentation

#### Modified Files
- `gamepad.h` - Updated to use CompositeHID instead of HID-Project directly
- `gamepad.cpp` - Replaced all Mouse/Keyboard calls with CompositeHID equivalents
- `latte-deck.ino` - Added CompositeHID::begin() call in setup() and prioritized NicoHood HID includes
- `upsDef.h` - Disabled DFRobot HID (`UPS_HID_NICOHOOD = 0`) and updated to use CompositeHID directly
- `ups_utils.cpp` - Removed PowerHID::begin() call (handled by CompositeHID::begin())
- `ups_ctrl.cpp` - Updated to use CompositeHID functions directly, removed conditional compilation
- `composite_hid.h` - Added comments about NicoHood HID library priority

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

- **Prioritizes NicoHood's HID-Project library** as the underlying framework over DFRobot's HID implementation
- **Disables DFRobot HID functionality** by setting `UPS_HID_NICOHOOD = 0` to prevent conflicts
- **Requires manual removal** of `HID.c` and `HID.h` files from DFRobot library to prevent compilation conflicts
- **Controls include order** to ensure NicoHood HID headers are loaded before DFRobot library headers
- Maintains compatibility with DFRobot LPUPS library for I2C communication only
- Follows USB HID specification for composite devices
- Report IDs are carefully chosen to avoid conflicts with standard HID devices
- Uses only the hardware communication features of the DFRobot library (I2C, battery reading)

## Windows Detection Issues and Fixes

### Problem: Keyboard and Mouse Not Automatically Detected by Windows

The original implementation had several issues that prevented Windows from properly recognizing the keyboard and mouse functionality:

1. **Mouse Descriptor Issues:**
   - Missing scroll wheel support in the HID descriptor
   - Incorrect report structure (3 bytes instead of 4)
   - Missing proper wheel axis definition

2. **Keyboard Descriptor Issues:**
   - Incorrect logical maximum for key codes (101 instead of 255)
   - Missing proper key array structure
   - Incomplete modifier key handling

3. **USB Device Recognition Issues:**
   - Missing proper USB device descriptor configuration
   - No initial report sending to establish device enumeration

### Solution: Fixed HID Descriptor and USB Configuration

#### Mouse Descriptor Fixes:
- Added scroll wheel support (`0x09, 0x38` - Usage (Wheel))
- Updated report structure to 4 bytes (buttons, x, y, wheel)
- Fixed report count to 3 for X, Y, and Wheel axes

#### Keyboard Descriptor Fixes:
- Changed logical maximum from 101 to 255 for proper key code range
- Updated usage maximum to 255 to support all standard key codes
- Maintained proper 6-key array structure

#### USB Configuration:
- Added `usb_config.h` with proper device descriptor settings
- Implemented initial report sending in `CompositeHID::begin()`
- Added proper device enumeration sequence

### Files Modified:
- `composite_hid.cpp` - Fixed HID descriptor and report functions
- `composite_hid.h` - Updated function signatures
- `usb_config.h` - New USB configuration file
- `latte-deck.ino` - Added USB configuration include

### Testing on Windows:
After these fixes, the device should be properly recognized by Windows as a composite HID device with:
- Power Device functionality (battery status)
- Mouse functionality (cursor movement, clicks, scroll wheel)
- Keyboard functionality (key presses)

**Note**: The joystick input is now handled through Mouse and Keyboard functions rather than a separate Gamepad device, which provides better Windows compatibility.

## Important Installation Note

The DFRobot LPUPS library includes its own HID implementation files (`HID.c` and `HID.h`) that conflict with the NicoHood HID-Project library. To resolve this:

1. Install both libraries through Arduino IDE Library Manager
2. Navigate to your Arduino libraries folder: `~/Arduino/libraries/DFRobot_LPUPS/`
3. Delete or rename the `HID.c` and `HID.h` files
4. This allows the NicoHood HID-Project library to be used exclusively for all HID functionality
