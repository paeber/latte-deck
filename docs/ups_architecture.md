# UPS Module Architecture

## Overview

The UPS (Uninterruptible Power Supply) module provides battery monitoring and Windows Power Device compatibility for the LatteDeck project. The module has been refactored into a clean, modular architecture with clear separation of concerns.

## Module Structure

### Core Modules

#### 1. `ups_common.h/cpp` - Shared Definitions and Utilities
**Purpose**: Central location for shared data structures, constants, and utilities.

**Responsibilities:**
- UPSStatus data structure definition
- Hardware configuration constants
- Debug printing utilities (eliminates duplication)
- Battery calculation functions (SoC from voltage)

**Key Components:**
```cpp
struct UPSStatus {
    uint16_t voltage_mV;
    uint16_t current_mA;
    uint16_t capacity_percent;
    uint16_t temperature_celsius;
    bool is_charging;
    bool is_connected;
    uint32_t last_update_ms;
    uint8_t raw_data[16];
};
```

#### 2. `ups_core.h/cpp` - Core UPS Functionality
**Purpose**: Hardware abstraction layer for UPS communication and battery data processing.

**Responsibilities:**
- DFRobot LPUPS library interface
- Raw battery data reading and parsing
- Hardware communication management
- Battery voltage/current extraction

**Key Features:**
- Non-blocking battery data reading
- Automatic communication error detection
- Raw data parsing with proper register interpretation
- Hardware abstraction for easy testing

#### 3. `ups_hid.h/cpp` - HID Power Device Interface
**Purpose**: Windows Power Device compatibility and status reporting.

**Responsibilities:**
- HID Power Device protocol implementation
- Windows battery status reporting
- Status LED control with advanced patterns
- HID descriptor management

**Key Features:**
- Windows-compatible battery reporting
- Intelligent LED patterns (charging fade, capacity blink)
- Efficient reporting (only when data changes)
- Complete HID Power Device feature set

#### 4. `ups_manager.h/cpp` - High-Level UPS Management
**Purpose**: Orchestrates all UPS functionality and provides public API.

**Responsibilities:**
- Coordinates UPS core and HID modules
- Main update loop management
- Public API for main application
- Configuration and initialization

**Key Features:**
- Non-blocking main update loop
- Automatic timing management
- Clean public interface
- Error handling and recovery

### Support Modules

#### 5. `ups_hid_core.h/cpp` - Low-Level HID Implementation
**Purpose**: USB HID protocol implementation (unchanged from original).

**Responsibilities:**
- USB HID protocol handling
- USB descriptor management
- Low-level USB communication

#### 6. `ups_power_device.h/cpp` - HID Power Device Protocol
**Purpose**: HID Power Device descriptor and protocol implementation (unchanged from original).

**Responsibilities:**
- HID Power Device descriptor
- Power Device protocol implementation
- Feature and report management

## Architecture Benefits

### 1. Clear Separation of Concerns
- **Core**: Hardware abstraction and data processing
- **HID**: Windows compatibility and status reporting  
- **Manager**: High-level orchestration and public API
- **Common**: Shared utilities and definitions

### 2. Reduced Dependencies
```
ups_manager.cpp -> ups_core.h + ups_hid.h
ups_hid.cpp -> ups_hid_core.h + ups_common.h
ups_core.cpp -> ups_common.h
```

### 3. Eliminated Code Duplication
- Single debug utility module
- Shared UPSStatus structure
- Common configuration constants
- Centralized battery calculations

### 4. Improved Maintainability
- Each module has a single, clear responsibility
- Easier to test individual components
- Simpler to modify or extend functionality
- Clear interfaces between modules

### 5. Better Reusability
- Core UPS functionality can be used without HID
- HID module can be adapted for other battery devices
- Manager provides clean public API
- Modular design allows selective inclusion

## Usage

### Basic Usage
```cpp
#include "ups_manager.h"

void setup() {
    if (setupUPS()) {
        Serial.println("UPS initialized successfully");
    }
}

void loop() {
    loopUPS();  // Non-blocking update
}
```

### Advanced Usage
```cpp
#include "ups_manager.h"

void loop() {
    ups.update();
    
    // Access current status
    const UPSStatus& status = ups.getStatus();
    if (status.is_connected) {
        Serial.print("Battery: ");
        Serial.print(status.capacity_percent);
        Serial.println("%");
    }
}
```

## Configuration

### Hardware Configuration
All hardware constants are defined in `ups_common.h`:
- Battery cell configuration (3-cell pack)
- Voltage ranges and limits
- LED pin assignments
- Timing parameters

### Feature Flags
Controlled via `config.h`:
- `DEBUG_PRINT_UPS`: Enable debug output
- `ENABLE_HID_POWER_DEVICE`: Enable Windows Power Device

## Debug Features

### Debug Output
When `DEBUG_PRINT_UPS` is enabled:
- Battery data parsing details
- Communication status
- HID reporting information
- LED state changes

### Status Monitoring
- Real-time battery status via Serial
- HID report contents
- Communication error detection
- LED pattern debugging

## Future Enhancements

### Potential Improvements
1. **Battery History**: Track battery usage over time
2. **Power Management**: Automatic power saving modes
3. **Advanced LED Patterns**: More sophisticated status indication
4. **Calibration**: User-configurable battery parameters
5. **Multiple Battery Support**: Support for different battery types

### Extensibility
The modular design makes it easy to:
- Add new battery types
- Implement different HID protocols
- Add power management features
- Integrate with other systems
