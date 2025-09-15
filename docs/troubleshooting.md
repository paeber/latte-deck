# Troubleshooting Guide

## Common Issues and Solutions

### Windows Driver Issues

#### "This device cannot start. (Code 10)" Error
1. **Open Device Manager** (Win+X → Device Manager)
2. **Find the problematic device** under "Human Interface Devices"
3. **Right-click** → "Update driver"
4. **Choose "Browse my computer for drivers"**
5. **Select "Let me pick from a list of available drivers"**
6. **Choose "HID-compliant device"** or **"USB Input Device"**
7. **Click "Next"** and **"Close"**

#### Device Shows as "Unknown Device"
- Use the same driver update process above
- Try different USB ports
- Check USB cable integrity

### HID Recognition Issues

#### Battery Status Not Showing
1. **Check UPS Connection**: Verify I2C wiring to UPS module
2. **Verify PID Value**: Ensure `THREE_BATTERIES_UPS_PID = 0x42AA` in `config.h`
3. **Check Serial Output**: Look for UPS initialization messages
4. **Test I2C Communication**: Use debug output to verify UPS communication

#### Mouse/Keyboard Not Working
1. **Check Gamepad Enable**: Ensure pin 4 is pulled LOW (enabled)
2. **Verify Joystick Wiring**: Check analog pin connections
3. **Test Individual Functions**: Use debug output to verify input readings
4. **Check HID Reports**: Verify reports are being sent via Serial Monitor

### Library Conflicts

#### Compilation Errors
- **Remove DFRobot HID files**: Delete `HID.c` and `HID.h` from DFRobot library folder
- **Check Include Order**: Ensure NicoHood HID library is included first
- **Verify Library Versions**: Use latest versions of both libraries

### Hardware Issues

#### Joystick Not Responding
1. **Check Pin Assignments**: Verify pins in `config.h` match hardware
2. **Test Analog Readings**: Use Serial Monitor to check raw values
3. **Calibrate Joysticks**: Ensure proper zero-point calibration
4. **Check Power Supply**: Verify stable 5V/3.3V supply

#### UPS Communication Failed
1. **Check I2C Wiring**: Verify SDA/SCL connections
2. **Test I2C Address**: Confirm `UPS_I2C_ADDRESS = 0x55`
3. **Check UPS Power**: Ensure UPS module is powered
4. **Verify PID**: Confirm correct product ID for your UPS model

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

## Debug Output

### Enable Debug Mode
Uncomment in `config.h`:
```cpp
#define DEBUG_PRINT_UPS 1
#define DEBUG_PRINT_GAMEPAD 1
```

### Expected Output
```
Starting LatteDeck...
Composite HID initialized
UPS init
UPS init done
UPS setup successful
Gamepad ready
LatteDeck ready!
```

### Troubleshooting Output
- **UPS errors**: Check I2C communication and PID value
- **Gamepad errors**: Verify pin assignments and joystick calibration
- **HID errors**: Check library conflicts and report ID configuration

## Performance Optimization

### Memory Usage
- Monitor RAM usage with different buffer sizes
- Adjust `OUTPUT_BUFFER_SIZE` and `DEBUG_BUFFER_SIZE` in `config.h`
- Use PROGMEM for large constant arrays

### Response Time
- Adjust `MIN_UPDATE_INTERVAL` for UPS reporting frequency
- Modify joystick sensitivity in `config.h`
- Optimize loop timing for better responsiveness

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
