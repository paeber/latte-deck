# LatteDeck Project Structure

This document provides an overview of the LatteDeck project organization and file structure.

## Root Directory

```
latte-deck/
├── README.md                    # Main project documentation
├── PROJECT_STRUCTURE.md         # This file
├── todo.md                      # Project TODO list
│
├── Arduino Core Files
├── latte-deck.ino              # Main Arduino sketch
├── config.h                    # Centralized configuration
├── usb_config.h                # USB descriptor configuration
├── hid_config.h                # HID configuration
│
├── Gamepad System
├── gamepad.h/cpp               # Main gamepad logic
├── gamepad_utils.h/cpp         # Gamepad utility functions
├── gamepad_assignment.h         # Button/key mappings
├── gamepad_pinout.h            # Hardware pin definitions
│
├── UPS System
├── ups_simple.h/cpp            # UPS battery monitoring
├── DFRobot_LPUPS.h/cpp         # DFRobot library integration
│
├── Battery Service (OS Integration)
├── battery-service/            # Complete battery monitoring system
│
└── Documentation
    └── docs/                   # Project documentation
```

## Battery Service Directory

The `battery-service/` directory contains all battery monitoring and OS integration components:

```
battery-service/
├── README.md                   # Battery service documentation
├── UBUNTU_DEPLOYMENT.md        # Ubuntu deployment guide
├── BATTERY_MONITORING.md       # JSON monitoring documentation
│
├── Rust Implementation (Recommended)
├── battery_service_rust/       # High-performance Rust service
│   ├── Cargo.toml             # Rust dependencies
│   ├── src/
│   │   ├── main.rs            # Main service implementation
│   │   └── ubuntu.rs          # Ubuntu-specific integration
│   └── target/                # Build output
│
├── C++ Implementation
├── battery_service.cpp         # Cross-platform C++ service
├── Makefile                   # Build configuration for C++ service
│
├── Python Scripts
├── battery_monitor.py          # Basic monitoring script
├── battery_system_integration.py # Advanced system integration
│
├── System Integration
├── latte-deck-battery.service  # systemd service file
├── install_ubuntu.sh          # Automated Ubuntu installation
├── test_ubuntu_service.sh     # Service testing and validation
```

## File Descriptions

### Arduino Core Files
- **`latte-deck.ino`**: Main Arduino sketch entry point
- **`config.h`**: Centralized configuration (pins, constants, features)
- **`usb_config.h`**: USB descriptor configuration for HID device
- **`hid_config.h`**: HID report configuration and report IDs

### Gamepad System
- **`gamepad.h/cpp`**: Main gamepad logic and state management
- **`gamepad_utils.h/cpp`**: Modular utility functions for joystick handling
- **`gamepad_assignment.h`**: Button-to-action mappings and key definitions
- **`gamepad_pinout.h`**: Hardware pin assignments and definitions

### UPS System
- **`ups_simple.h/cpp`**: Simplified UPS functionality with JSON reporting
- **`DFRobot_LPUPS.h/cpp`**: DFRobot library for UPS communication

### Battery Service
- **Rust Implementation**: High-performance, memory-safe service for Ubuntu
- **C++ Implementation**: Cross-platform service with direct OS integration
- **Python Scripts**: Development and testing tools
- **Installation Scripts**: Automated deployment and configuration
- **Documentation**: Complete deployment and usage guides

## Development Workflow

### Arduino Development
1. Edit Arduino files in root directory
2. Use Arduino IDE to compile and upload
3. Test with serial monitor

### Battery Service Development
1. **Rust**: Edit files in `battery-service/battery_service_rust/`
2. **C++**: Edit `battery-service/battery_service.cpp`
3. **Python**: Edit scripts in `battery-service/`
4. Test with provided test scripts

### Documentation
- Update relevant `.md` files in `docs/` or `battery-service/`
- Keep README files current with project changes

## Build Instructions

### Arduino
```bash
# Use Arduino IDE or command line
arduino-cli compile --fqbn arduino:avr:leonardo latte-deck.ino
```

### Battery Service (Rust)
```bash
cd battery-service/battery_service_rust
cargo build --release
```

### Battery Service (C++)
```bash
cd battery-service
make
```

## Installation

### Ubuntu (Recommended)
```bash
cd battery-service
sudo ./install_ubuntu.sh
```

### Manual Installation
```bash
# Build and install service
cd battery-service/battery_service_rust
cargo build --release
sudo cp target/release/latte-deck-battery-service /usr/local/bin/
sudo cp ../latte-deck-battery.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable latte-deck-battery
sudo systemctl start latte-deck-battery
```

## Testing

### Arduino Testing
- Use Arduino IDE serial monitor
- Check for JSON output: `UPS_JSON:{"capacity_percent":85,...}`

### Battery Service Testing
```bash
cd battery-service
sudo ./test_ubuntu_service.sh
```

### Manual Testing
```bash
# Test serial communication
sudo minicom -D /dev/ttyACM0 -b 115200

# Test battery device
cat /sys/class/power_supply/latte_deck_battery/capacity
```

## Maintenance

### Updating Arduino Code
1. Edit Arduino files
2. Upload to device
3. Test functionality

### Updating Battery Service
1. Edit service code
2. Rebuild service
3. Restart service: `sudo systemctl restart latte-deck-battery`

### Updating Documentation
1. Edit relevant `.md` files
2. Test examples and commands
3. Update version information if needed
