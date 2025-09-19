# LatteDeck User Guide

## Quick Start

### Hardware Setup
1. **Connect UPS Module** - I2C wiring (SDA/SCL to LattePanda)
2. **Connect Gamepad** - Analog joysticks and buttons to defined pins
3. **Upload Code** - Flash `latte-deck.ino` to Arduino Leonardo
4. **Enable Gamepad** - Pull pin 4 LOW to enable gamepad functionality

### Software Setup
1. **Install Libraries**:
   - DFRobot LPUPS library
   - HID-Project library
2. **Configure** - Edit `config.h` for your hardware setup
3. **Upload** - Compile and upload to device

## Configuration

### Basic Configuration

Edit `config.h` to match your hardware:

```cpp
// UPS Configuration
#define UPS_I2C_ADDRESS             0x55
#define THREE_BATTERIES_UPS_PID     0x42AA
#define UPS_STATUS_LED              13

// Gamepad Enable Pin
#define PIN_GAMEPAD_ENABLE          4

// Joystick Pins
#define PIN_JOYSTICK_L_X            A5
#define PIN_JOYSTICK_L_Y            A4
#define PIN_JOYSTICK_R_X            A3
#define PIN_JOYSTICK_R_Y            A2
```

### Feature Flags

Enable/disable features in `config.h`:

```cpp
// Enable UPS battery monitoring
#define ENABLE_HID_POWER_DEVICE     1

// Enable mouse/keyboard emulation
#define ENABLE_MOUSE_KEYBOARD       1

// Debug output (uncomment to enable)
//#define DEBUG_PRINT_UPS 1
//#define DEBUG_PRINT_GAMEPAD 1
```

### Button Customization

Modify button assignments in `gamepad_assignment.h`:

```cpp
// Change L2 button to F key
#define ACTION_BTN_L2                  'f'

// Disable L4 button
#define ACTION_BTN_L4                  ACTION_NONE

// Change sprint key
#define ACTION_JOYSTICK_L_MAX          'e'
```

## Usage

### Gamepad Controls

#### Left Side (Movement)
- **Left Joystick**: WASD movement controls
- **L1-L4 Buttons**: Game actions (Q, 1, 2, Right Click)

#### Right Side (Mouse)
- **Right Joystick**: Mouse cursor movement
- **R1-R4 Buttons**: Mouse clicks and actions (Left Click, Space, R, E)

### UPS Monitoring

The UPS system provides:
- **Battery percentage** in Windows system tray
- **Charging status** indication
- **Status LED** visual feedback
- **Real-time monitoring** via I2C

## Troubleshooting

### Common Issues

#### Device Not Recognized
**Symptoms**: Device shows as "Unknown Device" in Device Manager

**Solutions**:
1. Update driver manually:
   - Device Manager → Right-click device → "Update driver"
   - Choose "Browse my computer for drivers"
   - Select "HID-compliant device"
2. Try different USB port
3. Check USB cable integrity

#### Battery Status Not Showing
**Symptoms**: No battery icon in Windows system tray

**Solutions**:
1. Check UPS connection (I2C wiring)
2. Verify PID value: `THREE_BATTERIES_UPS_PID = 0x42AA`
3. Enable debug output: `#define DEBUG_PRINT_UPS 1`
4. Check serial monitor for UPS initialization messages

#### Gamepad Not Working
**Symptoms**: Joysticks/buttons don't respond

**Solutions**:
1. Check gamepad enable pin (pin 4 should be LOW)
2. Verify joystick wiring and pin assignments
3. Enable debug output: `#define DEBUG_PRINT_GAMEPAD 1`
4. Test individual functions via serial monitor

#### Compilation Errors
**Symptoms**: Multiple definition errors or library conflicts

**Solutions**:
1. Remove DFRobot HID files (`HID.c` and `HID.h`)
2. Ensure `hid_config.h` is included before `HID-Project.h`
3. Use latest library versions
4. Clean and rebuild project

### Debug Output

Enable debug output to troubleshoot issues:

```cpp
// In config.h
#define DEBUG_PRINT_UPS 1
#define DEBUG_PRINT_GAMEPAD 1
```

#### Expected Debug Output
```
Starting LatteDeck...
Composite HID initialized
UPS init
UPS init done
UPS setup successful
Gamepad ready
LatteDeck ready!
```

#### UPS Debug Output
```
UPS Status - Voltage: 12600 mV, Current: 500 mA, Capacity: 85%, Charging: Yes
UPS Report - Capacity: 85%, Voltage: 12600mV, Charging: Yes
```

### Windows Battery Verification

#### Method 1: Device Manager
1. Open Device Manager (Win+X → Device Manager)
2. Check "Batteries" section for "HID UPS Battery"
3. Verify device status shows "working properly"

#### Method 2: PowerShell Query
```powershell
Get-CimInstance -ClassName Win32_Battery | Select-Object Name, BatteryStatus, EstimatedChargeRemaining
```

Expected output:
```
Name                : HID UPS Battery
BatteryStatus       : 2 (Discharging) or 1 (Charging)
EstimatedChargeRemaining : 85
```

#### Method 3: System Tray
- Look for battery icon in system tray
- Hover to see percentage and status
- Should update every 5 seconds

### Performance Optimization

#### Memory Usage
- Monitor RAM usage with different buffer sizes
- Adjust `OUTPUT_BUFFER_SIZE` and `DEBUG_BUFFER_SIZE` in `config.h`
- Use PROGMEM for large constant arrays

#### Response Time
- Adjust `MIN_UPDATE_INTERVAL` for UPS reporting frequency
- Modify joystick sensitivity in `config.h`
- Optimize loop timing for better responsiveness

## Testing Checklist

### Basic Functionality
- [ ] Device appears in Device Manager without errors
- [ ] Battery status shows in Windows Power settings
- [ ] Right joystick moves mouse cursor
- [ ] Left joystick types WASD keys
- [ ] Joystick button presses register (mouse click/space)

### Advanced Testing
- [ ] Battery percentage updates in real-time
- [ ] Mouse movement is smooth and responsive
- [ ] Keyboard input works with multiple keys
- [ ] Gamepad enable/disable works correctly
- [ ] All interfaces work simultaneously

### Long-term Testing
- [ ] Run for 24+ hours without crashes
- [ ] Battery monitoring remains accurate
- [ ] No memory leaks or performance degradation
- [ ] USB connection remains stable

## Getting Help

### Information to Provide
1. **Hardware**: LattePanda model, UPS module type
2. **Software**: Arduino IDE version, library versions
3. **Error Messages**: Exact error text and when it occurs
4. **Debug Output**: Serial monitor output during initialization
5. **System**: Windows version, USB port type

### Common Solutions
- **Restart Arduino IDE** after library changes
- **Try different USB cable** for connection issues
- **Reset LattePanda** if device becomes unresponsive
- **Check Windows Updates** for driver compatibility
