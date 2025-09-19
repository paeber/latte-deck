# LatteDeck Battery Service

This directory contains all battery monitoring and system integration components for the LatteDeck project.

## Overview

The LatteDeck Battery Service provides comprehensive battery monitoring and OS integration for the LatteDeck hardware. It includes multiple implementation options optimized for different use cases and operating systems.

## Components

### Rust Implementation (Recommended)
- **`battery_service_rust/`** - High-performance Rust service
  - Native Ubuntu UPower integration
  - Minimal resource usage (2-8MB RAM)
  - Memory-safe operation
  - Async I/O for efficient serial communication
  - Single binary deployment
  - Requires Rust 1.80+ (installed automatically)

### C++ Implementation
- **`battery_service.cpp`** - C++ service implementation
  - Cross-platform support
  - Direct OS API access
  - Minimal memory usage (1-5MB RAM)
  - System service integration
- **`Makefile`** - Build configuration for C++ service

### Python Scripts
- **`battery_monitor.py`** - Basic monitoring script
  - Real-time battery status display
  - Command-line interface
  - Good for testing and development

- **`battery_system_integration.py`** - Advanced system integration
  - System notifications
  - Logging and monitoring
  - Status file creation

### System Integration
- **`latte-deck-battery.service`** - systemd service file
- **`install_ubuntu.sh`** - Automated Ubuntu installation
- **`test_ubuntu_service.sh`** - Service testing and validation

### Documentation
- **`UBUNTU_DEPLOYMENT.md`** - Complete Ubuntu deployment guide
- **`BATTERY_MONITORING.md`** - JSON monitoring documentation

## Quick Start

### Ubuntu (Recommended)
```bash
# Automated installation
sudo ./install_ubuntu.sh

# Test the service
sudo ./test_ubuntu_service.sh

# Check battery status
cat /sys/class/power_supply/latte_deck_battery/capacity
```

### Manual Installation

#### Rust Service (Recommended)
```bash
# Build Rust service
cd battery_service_rust
cargo build --release

# Install service
sudo cp target/release/latte-deck-battery-service /usr/local/bin/
sudo cp ../latte-deck-battery.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable latte-deck-battery
sudo systemctl start latte-deck-battery
```

#### C++ Service
```bash
# Build C++ service
make

# Install service
sudo make install
sudo systemctl start latte-deck-battery
```

### Python Monitoring
```bash
# Basic monitoring
python3 battery_monitor.py /dev/ttyACM0 115200

# System integration
python3 battery_system_integration.py /dev/ttyACM0 115200
```

## Architecture

### Data Flow
```
LatteDeck Hardware
    ↓ (Serial JSON)
Rust/C++ Service
    ↓ (OS Integration)
Ubuntu UPower
    ↓ (System Integration)
Desktop Environment
    ↓ (User Interface)
System Tray / Power Settings
```

### Service Components
- **Serial Reader**: Reads JSON data from LatteDeck
- **Data Parser**: Parses and validates battery data
- **OS Integration**: Updates system power management
- **Device Manager**: Creates virtual battery device
- **Logging**: Comprehensive service logging

## Features

### Battery Monitoring
- Real-time voltage, current, and capacity monitoring
- Temperature tracking
- Charging/discharging status
- Connection status monitoring
- Error handling and recovery

### OS Integration
- Native Ubuntu UPower integration
- System tray battery display
- Power management integration
- Desktop notifications
- System service management

### Performance
- **Rust**: 2-8MB RAM, <1% CPU
- **C++**: 1-5MB RAM, <1% CPU
- **Python**: 20-50MB RAM, 2-5% CPU

## Configuration

### Serial Port
Default: `/dev/ttyACM0` @ 115200 baud
- Configurable via systemd service override
- Automatic port detection
- udev rules for permissions

### Logging
- **Rust**: `RUST_LOG=info` (configurable)
- **C++**: syslog integration
- **Python**: File and console logging

### Resource Limits
- Memory: 10MB limit
- CPU: 10% quota
- Security: No new privileges, protected system

## Troubleshooting

### Common Issues
1. **Service won't start**: Check serial port and permissions
2. **No battery device**: Verify service logs and udev rules
3. **UPower integration**: Restart UPower service
4. **Serial communication**: Test with `minicom` or `screen`

### Debug Mode
```bash
# Enable debug logging
sudo systemctl edit latte-deck-battery
[Service]
Environment=RUST_LOG=debug

# Restart service
sudo systemctl daemon-reload
sudo systemctl restart latte-deck-battery

# Monitor logs
sudo journalctl -u latte-deck-battery -f
```

### Testing
```bash
# Run test suite
sudo ./test_ubuntu_service.sh

# Manual testing
sudo -u latte-deck /usr/local/bin/latte-deck-battery-service /dev/ttyACM0 115200
```

## Development

### Building
```bash
# Rust (Recommended)
cd battery_service_rust
cargo build --release

# C++
make

# Or manually:
# g++ -std=c++17 -O2 -o battery_service battery_service.cpp -lpthread
```

### Testing
```bash
# Unit tests
cd battery_service_rust
cargo test

# Integration tests
sudo ./test_ubuntu_service.sh
```

## License

This project is licensed under the MIT License. See the main project LICENSE file for details.

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review service logs: `sudo journalctl -u latte-deck-battery -f`
3. Test hardware connection: `sudo minicom -D /dev/ttyACM0 -b 115200`
4. Verify service status: `sudo systemctl status latte-deck-battery`
