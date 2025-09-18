# Simplified UPS Architecture

## Overview

The UPS implementation has been streamlined from 10 complex files to 2 simple files to eliminate crashes and improve maintainability.

## Problems with Original Architecture

### 1. **Memory Management Issues**
- Multiple `new`/`delete` pairs without proper error handling
- Memory leaks in HID core USB operations
- Heap fragmentation from frequent allocations

### 2. **USB Communication Problems**
- Blocking USB operations without timeout protection
- Complex HID descriptor management
- Race conditions in USB endpoint handling

### 3. **Overly Complex Structure**
- 10 files with circular dependencies
- Scattered responsibilities across multiple classes
- Difficult to debug and maintain

## New Simplified Architecture

### Files Structure
```
ups_simple.h    - Header file with class definition and configuration
ups_simple.cpp  - Complete implementation in single file
```

### Key Improvements

#### 1. **Consolidated Functionality**
- Single `SimpleUPS` class handles all UPS operations
- Eliminated complex inheritance and multiple managers
- Clear separation of concerns within one class

#### 2. **Better Memory Management**
- Minimal dynamic allocation (only for DFRobot library)
- Stack allocation for temporary buffers
- Proper error handling for all allocations

#### 3. **Simplified HID Reporting**
- Removed complex HID Power Device implementation
- Simple serial output for debugging
- Can be extended with basic HID if needed

#### 4. **Robust Error Handling**
- Timeout protection for all operations
- Graceful degradation on failures
- Conservative reporting intervals

## Usage

### Basic Setup
```cpp
#include "ups_simple.h"

void setup() {
    if (setupSimpleUPS()) {
        Serial.println("UPS initialized successfully");
    } else {
        Serial.println("UPS initialization failed");
    }
}

void loop() {
    loopSimpleUPS(); // Call this in your main loop
    
    // Access UPS status
    if (simple_ups.isConnected()) {
        uint16_t capacity = simple_ups.getCapacityPercent();
        bool charging = simple_ups.isCharging();
        uint16_t voltage = simple_ups.getVoltage();
        
        // Use the data as needed
    }
}
```

### Status LED Behavior
- **Charging**: Fading in/out every 2 seconds
- **Discharging**: Blink pattern where on-time represents battery percentage
- **Disconnected**: Fast blink (500ms cycle)

### Debug Output
Enable debug output by setting `DEBUG_PRINT_UPS 1` in `config.h`:
```
UPS Status - Voltage: 12600 mV, Current: 500 mA, Capacity: 85%, Charging: Yes, Connected: Yes
UPS Report - Capacity: 85%, Voltage: 12600mV, Charging: Yes, Connected: Yes
```

## Configuration

### Hardware Configuration
```cpp
#define UPS_STATUS_LED              13      // Status LED pin
#define UPS_I2C_ADDRESS             0x55    // I2C address for UPS module
#define N_CELLS_PACK                3       // 3 cells in series
#define MIN_CELL_VOLTAGE            2600    // Minimum cell voltage (mV)
#define MAX_CELL_VOLTAGE            4200    // Maximum cell voltage (mV)
```

### Timing Configuration
- Battery data update: 1 second
- LED update: 50ms
- HID reporting: 30-60 seconds (adaptive based on failures)

## Migration from Old Architecture

### Replace Old Includes
```cpp
// Old
#include "ups_manager.h"

// New
#include "ups_simple.h"
```

### Replace Old Function Calls
```cpp
// Old
UPSManager ups;
ups.begin();
ups.update();
uint16_t capacity = ups.getCapacityPercent();

// New
SimpleUPS simple_ups; // Global instance
setupSimpleUPS();     // Initialization
loopSimpleUPS();      // Update loop
uint16_t capacity = simple_ups.getCapacityPercent();
```

## Future Enhancements

### 1. **Basic HID Support**
If Windows battery display is needed, a simplified HID implementation can be added:
```cpp
void SimpleUPS::reportBatteryStatus() {
    // Add basic HID report here
    // Much simpler than the original complex implementation
}
```

### 2. **Configuration Options**
Add runtime configuration for:
- Report intervals
- LED behavior
- Debug output levels

### 3. **Advanced Features**
- Battery health monitoring
- Temperature compensation
- Multiple battery support

## Benefits

1. **Reliability**: Eliminated crash-causing memory issues
2. **Maintainability**: Single file implementation is easier to debug
3. **Performance**: Reduced memory usage and faster execution
4. **Simplicity**: Clear, straightforward code structure
5. **Extensibility**: Easy to add features without breaking existing code

## Testing

The simplified implementation should be tested with:
1. Battery connected/disconnected scenarios
2. Charging/discharging cycles
3. Long-term operation (24+ hours)
4. Memory usage monitoring
5. USB communication stability
