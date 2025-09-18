# UPS Module Refactoring Plan

## Current Issues

### 1. Scattered Responsibilities
- **ups.h/cpp**: Basic battery calculations and constants
- **ups_control.h/cpp**: Main UPS control logic, status LED, DFRobot library interface
- **ups_hid.h/cpp**: HID Power Device reporting layer
- **ups_hid_core.h/cpp**: Low-level HID implementation
- **ups_power_device.h/cpp**: HID Power Device descriptor and protocol

### 2. Duplicate Code
- Debug printing functions duplicated across `ups_control.cpp` and `ups_hid.cpp`
- Similar status reporting patterns in multiple files
- Repeated UPS status structure usage

### 3. Complex Dependencies
```
ups_control.cpp -> ups_hid.h -> ups_power_device.h -> ups_hid_core.h
                -> ups.h
                -> config.h
```

## Proposed New Structure

### Core Modules (3 files)

#### 1. `ups_core.h/cpp` - Core UPS Functionality
**Responsibilities:**
- Battery voltage/current reading from DFRobot library
- SoC (State of Charge) calculations
- Basic UPS status structure
- Hardware abstraction for UPS communication

**Public Interface:**
```cpp
class UPSCore {
public:
    bool begin();
    bool readBatteryData(UPSStatus& status);
    uint16_t calculateSoC(uint16_t voltage_mV, uint16_t current_mA, bool charging);
    bool isConnected() const;
};
```

#### 2. `ups_hid.h/cpp` - HID Power Device Interface
**Responsibilities:**
- HID Power Device protocol implementation
- Windows battery status reporting
- HID descriptor management
- Status LED control

**Public Interface:**
```cpp
class UPSHID {
public:
    bool begin();
    void reportBatteryStatus(const UPSStatus& status);
    void updateStatusLED(const UPSStatus& status);
    void setDebugOutput(bool enable);
};
```

#### 3. `ups_manager.h/cpp` - High-Level UPS Management
**Responsibilities:**
- Orchestrates UPS core and HID functionality
- Main update loop
- Configuration management
- Public API for main application

**Public Interface:**
```cpp
class UPSManager {
public:
    bool begin();
    void update();
    const UPSStatus& getStatus() const;
    bool isInitialized() const;
};

// Global instance and convenience functions
extern UPSManager ups;
bool setupUPS();
void loopUPS();
```

### Support Modules (2 files)

#### 4. `ups_hid_core.h/cpp` - Low-Level HID Implementation
**Responsibilities:**
- USB HID protocol implementation
- USB descriptor handling
- Low-level USB communication

#### 5. `ups_common.h` - Shared Definitions
**Responsibilities:**
- Common data structures (UPSStatus)
- Constants and configuration
- Debug utilities
- Shared includes

## Benefits of New Structure

### 1. Clear Separation of Concerns
- **Core**: Hardware abstraction and battery calculations
- **HID**: Windows compatibility and status reporting
- **Manager**: High-level orchestration and public API

### 2. Reduced Dependencies
```
ups_manager.cpp -> ups_core.h + ups_hid.h
ups_hid.cpp -> ups_hid_core.h + ups_common.h
ups_core.cpp -> ups_common.h
```

### 3. Eliminated Duplication
- Single debug utility module
- Shared UPSStatus structure
- Common configuration constants

### 4. Better Maintainability
- Each module has a single, clear responsibility
- Easier to test individual components
- Simpler to modify or extend functionality

### 5. Improved Reusability
- Core UPS functionality can be used without HID
- HID module can be adapted for other battery devices
- Manager provides clean public API

## Migration Strategy

1. **Phase 1**: Create new module files with clean interfaces
2. **Phase 2**: Implement core functionality in new structure
3. **Phase 3**: Update main application to use new API
4. **Phase 4**: Remove old files and clean up includes
5. **Phase 5**: Update documentation

## File Organization

```
ups/
├── ups_common.h          # Shared definitions and utilities
├── ups_core.h/cpp        # Core UPS functionality
├── ups_hid.h/cpp         # HID Power Device interface
├── ups_hid_core.h/cpp    # Low-level HID implementation
└── ups_manager.h/cpp     # High-level UPS management
```

This structure provides clear boundaries, eliminates duplication, and makes the code much more maintainable.
