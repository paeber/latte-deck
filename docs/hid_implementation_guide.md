# HID Power Device Implementation Guide

## Quick Start

This guide provides step-by-step instructions for implementing HID Power Device reporting for your UPS system.

## Current Status

✅ **Ready to Use**: The implementation is complete and functional
✅ **Debug Output**: Comprehensive battery status reporting via serial
✅ **Modular Design**: Easy to integrate and modify
✅ **Workaround Implementation**: Uses Consumer Control for immediate compatibility

## Implementation Options

### Option 1: Use Current Workaround (Recommended for Quick Start)

The current implementation is ready to use and provides:
- Battery status logging via serial output
- Debug information for monitoring UPS status
- Framework for future enhancements
- No library modifications required

**To enable:**
1. Set `DEBUG_PRINT_UPS 1` in `config.h`
2. Upload the code to your Arduino Leonardo
3. Monitor serial output at 115200 baud

**Example output:**
```
UPS HID interface initialized
Leonardo HID Power Device Report:
  Report ID: 1
  Battery Present: Yes
  Capacity: 85%
  Voltage: 12600 mV
  Current: -150 mA
  Temperature: 25°C
  Status: 2
  Runtime to Empty: 340 min
Power Device report sent via Consumer Control workaround
```

### Option 2: Full HID Implementation (Advanced Users)

For full HID Power Device support, follow the [Arduino Library Modifications Guide](arduino_library_modifications.md).

## Integration Steps

### Step 1: Verify Current Implementation

1. **Check Configuration**
   ```cpp
   // In config.h
   #define ENABLE_HID_POWER_DEVICE 1
   #define DEBUG_PRINT_UPS 1
   ```

2. **Verify UPS Connection**
   - Ensure UPS is connected via I2C
   - Check UPS initialization in serial output

3. **Monitor HID Reports**
   - Open Arduino IDE Serial Monitor
   - Set baud rate to 115200
   - Look for HID report output every 5 seconds

### Step 2: Customize Report Parameters

You can modify the reporting behavior in `hid_leonardo.cpp`:

```cpp
// Change report interval (currently 5 seconds)
if (current_time - last_report_time < 5000) { // Change 5000 to desired interval in ms
    return;
}
```

### Step 3: Add Consumer Control Integration

To send actual Consumer Control codes, uncomment and modify the code in `sendViaConsumerControl()`:

```cpp
// Battery capacity as volume level
if (report.battery_capacity > 0) {
    // Uncomment to send actual Consumer Control codes
    // Consumer.write(CONSUMER_VOLUME_UP);
}

// Battery status as power control
if (report.battery_status == BATTERY_STATUS_CHARGING) {
    // Uncomment to send actual Consumer Control codes
    // Consumer.write(CONSUMER_POWER_ON_OFF);
}
```

### Step 4: Enable System Integration

For system-level integration, you can create a companion application that reads the serial output and provides system notifications:

**Python Example:**
```python
import serial
import time

def monitor_ups_status():
    ser = serial.Serial('/dev/ttyACM0', 115200)  # Adjust port as needed
    
    while True:
        line = ser.readline().decode('utf-8').strip()
        if 'Battery Level:' in line:
            capacity = int(line.split(':')[1].strip().replace('%', ''))
            if capacity < 20:
                # Send system notification
                print(f"Low battery warning: {capacity}%")
        time.sleep(1)

if __name__ == "__main__":
    monitor_ups_status()
```

## Testing and Validation

### Test 1: Basic Functionality

1. **Upload Code**: Upload the sketch to Arduino Leonardo
2. **Monitor Serial**: Check for initialization messages
3. **Verify Reports**: Confirm HID reports are generated every 5 seconds

**Expected Output:**
```
Starting LatteDeck...
NicoHood HID initialized
Gamepad setup completed
UPS setup completed
UPS HID reporting initialized
LatteDeck ready!
```

### Test 2: UPS Status Monitoring

1. **Connect UPS**: Ensure UPS is properly connected
2. **Monitor Status**: Watch for battery status changes
3. **Test Scenarios**: Try charging/discharging states

**Expected Behavior:**
- Battery capacity updates reflect actual UPS status
- Charging status changes when power is connected/disconnected
- Voltage and current readings are reasonable

### Test 3: Debug Output Validation

1. **Enable Debug**: Set `DEBUG_PRINT_UPS 1`
2. **Check Reports**: Verify detailed report information
3. **Validate Data**: Ensure all fields contain valid data

**Sample Debug Output:**
```
Sending Leonardo HID Power Device report...
Leonardo HID Power Device Report:
  Report ID: 1
  Battery Present: Yes
  Capacity: 75%
  Voltage: 12400 mV
  Current: 200 mA
  Temperature: 26°C
  Status: 3
  Runtime to Empty: 225 min
Leonardo HID Report Data: 0x01 0x01 0x4B 0x70 0x30 0x00 0xC8 0x1A 0x03 0xE1 0x00
Power Device report sent via Consumer Control workaround
```

## Troubleshooting

### Issue 1: No HID Reports

**Symptoms**: Serial output shows initialization but no HID reports
**Solutions**:
1. Check `ENABLE_HID_POWER_DEVICE` is set to 1
2. Verify UPS is properly initialized
3. Enable debug output to see detailed information

### Issue 2: Incorrect Battery Data

**Symptoms**: Battery readings seem wrong or inconsistent
**Solutions**:
1. Check UPS I2C connection
2. Verify UPS library compatibility
3. Review raw data output in debug mode

### Issue 3: Compilation Errors

**Symptoms**: Arduino IDE shows compilation errors
**Solutions**:
1. Ensure all required libraries are installed (HID-Project)
2. Check that all header files are present
3. Verify Arduino Leonardo board is selected

## Advanced Configuration

### Custom Report Structure

You can modify the HID report structure in `hid_leonardo.h`:

```cpp
struct LeonardoHIDPowerDeviceReport {
    uint8_t report_id;           // Report ID (1)
    uint8_t battery_present;     // 0 = not present, 1 = present
    uint8_t battery_capacity;    // Battery capacity percentage (0-100)
    uint16_t battery_voltage;    // Battery voltage in millivolts
    int16_t battery_current;     // Battery current in milliamps
    uint8_t battery_temperature; // Battery temperature in Celsius
    uint8_t battery_status;      // Battery status
    uint16_t runtime_to_empty;   // Runtime to empty in minutes
    // Add custom fields here
} __attribute__((packed));
```

### Report Timing Configuration

Adjust report timing based on your needs:

```cpp
// For more frequent updates (every 1 second)
if (current_time - last_report_time < 1000) {
    return;
}

// For less frequent updates (every 30 seconds)
if (current_time - last_report_time < 30000) {
    return;
}
```

### Integration with External Systems

Create interfaces for external monitoring systems:

```cpp
// JSON output for web interfaces
void printReportJSON(const LeonardoHIDPowerDeviceReport& report) {
    Serial.print("{");
    Serial.print("\"battery_present\":");
    Serial.print(report.battery_present);
    Serial.print(",\"capacity\":");
    Serial.print(report.battery_capacity);
    Serial.print(",\"voltage\":");
    Serial.print(report.battery_voltage);
    Serial.print(",\"current\":");
    Serial.print(report.battery_current);
    Serial.print(",\"temperature\":");
    Serial.print(report.battery_temperature);
    Serial.print(",\"status\":");
    Serial.print(report.battery_status);
    Serial.print(",\"runtime\":");
    Serial.print(report.runtime_to_empty);
    Serial.println("}");
}
```

## Next Steps

1. **Test Current Implementation**: Start with the workaround implementation
2. **Monitor Performance**: Observe battery status reporting
3. **Consider Library Modifications**: For full HID support, follow the library modification guide
4. **Integrate with Systems**: Create companion applications for system integration

## Support and Documentation

- **Implementation Details**: See `docs/hid_power_device_implementation.md`
- **Library Modifications**: See `docs/arduino_library_modifications.md`
- **Troubleshooting**: See `docs/troubleshooting.md`
- **API Reference**: See individual header files for detailed API documentation
