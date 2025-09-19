# LatteDeck Battery Monitoring

This document describes the JSON-based battery monitoring system for LatteDeck, which provides a practical alternative to the complex HID Power Device implementation.

## Overview

The LatteDeck now outputs battery status data in JSON format via serial communication, which can be easily parsed by Python scripts running on the host operating system.

## JSON Data Format

The Arduino outputs battery data in the following JSON format:

```json
{
  "timestamp": 12345,
  "voltage_mV": 12600,
  "current_mA": 500,
  "capacity_percent": 85,
  "temperature_celsius": 25,
  "is_charging": true,
  "is_connected": true,
  "last_update_ms": 12345678,
  "consecutive_failures": 0
}
```

### Field Descriptions

- `timestamp`: Arduino millis() timestamp
- `voltage_mV`: Battery pack voltage in millivolts
- `current_mA`: Battery current in milliamps (positive = charging, negative = discharging)
- `capacity_percent`: Battery capacity percentage (0-100)
- `temperature_celsius`: Battery temperature in Celsius
- `is_charging`: Boolean indicating if battery is charging
- `is_connected`: Boolean indicating if UPS module is connected
- `last_update_ms`: Timestamp of last successful data read
- `consecutive_failures`: Number of consecutive I2C communication failures

## Configuration

### Arduino Configuration

In `ups_simple.h`, you can configure:

```cpp
// JSON Reporting Configuration
#define UPS_JSON_REPORT_INTERVAL    5000    // JSON report interval (ms)
#define UPS_JSON_ENABLED            1       // Enable JSON reporting
```

### Serial Output

The Arduino outputs data with the prefix `UPS_JSON:` followed by the JSON data:

```
UPS_JSON:{"timestamp":12345,"voltage_mV":12600,"current_mA":500,"capacity_percent":85,"temperature_celsius":25,"is_charging":true,"is_connected":true,"last_update_ms":12345678,"consecutive_failures":0}
```

## Python Scripts

### 1. Basic Battery Monitor (`battery_monitor.py`)

A simple script to monitor and display battery status:

```bash
# Install required Python packages
pip install pyserial

# Run the monitor
python3 battery_monitor.py /dev/ttyACM0 115200

# Get single reading
python3 battery_monitor.py /dev/ttyACM0 115200 --single

# Custom update interval
python3 battery_monitor.py /dev/ttyACM0 115200 --interval 2.0
```

**Features:**
- Real-time battery status display
- Color-coded capacity indicators
- Charging/discharging status
- Connection status monitoring
- Error handling and timeout detection

### 2. System Integration (`battery_system_integration.py`)

Advanced script for system-level integration:

```bash
# Run system integration
python3 battery_system_integration.py /dev/ttyACM0 115200
```

**Features:**
- System notifications (Linux/Windows)
- Low battery warnings
- Logging to file
- Status file creation for other scripts
- Rate-limited notifications

## Usage Examples

### Basic Monitoring

```python
import serial
import json

# Connect to LatteDeck
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

while True:
    line = ser.readline().decode('utf-8').strip()
    if line.startswith("UPS_JSON:"):
        data = json.loads(line[9:])
        print(f"Battery: {data['capacity_percent']}% | {data['voltage_mV']/1000:.2f}V")
```

### System Integration

```python
from battery_system_integration import BatterySystemIntegration

# Create integration instance
integration = BatterySystemIntegration('/dev/ttyACM0', 115200)

# Connect and monitor
if integration.connect():
    integration.monitor()
```

## Integration with System Services

### Linux Systemd Service

Create `/etc/systemd/system/latte-deck-battery.service`:

```ini
[Unit]
Description=LatteDeck Battery Monitor
After=multi-user.target

[Service]
Type=simple
User=your_username
WorkingDirectory=/path/to/latte-deck
ExecStart=/usr/bin/python3 /path/to/latte-deck/battery_system_integration.py /dev/ttyACM0
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

Enable and start the service:

```bash
sudo systemctl enable latte-deck-battery.service
sudo systemctl start latte-deck-battery.service
```

### Windows Task Scheduler

1. Open Task Scheduler
2. Create Basic Task
3. Set trigger (e.g., "At startup")
4. Set action to start `battery_system_integration.py`

## Troubleshooting

### Common Issues

1. **No data received**
   - Check serial port permissions
   - Verify correct port name (`/dev/ttyACM0`, `COM3`, etc.)
   - Ensure Arduino is connected and running

2. **JSON parsing errors**
   - Check baud rate (should be 115200)
   - Verify Arduino code is uploaded correctly
   - Look for corrupted serial data

3. **Permission denied**
   - Add user to dialout group: `sudo usermod -a -G dialout $USER`
   - Log out and back in

### Debug Mode

Enable debug output in Arduino by uncommenting in `config.h`:

```cpp
#define DEBUG_PRINT_UPS 1
```

This will show additional debug information in the serial monitor.

## Benefits

1. **Simple Implementation**: No complex HID Power Device code
2. **Cross-Platform**: Works on Linux, Windows, macOS
3. **Extensible**: Easy to add new features and integrations
4. **Reliable**: No memory issues or crashes
5. **Flexible**: Can be integrated with any system monitoring solution

## Future Enhancements

- Web dashboard for remote monitoring
- MQTT integration for IoT systems
- Database logging for historical data
- Integration with system power management
- Custom notification rules and thresholds
