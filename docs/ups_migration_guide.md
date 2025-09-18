# UPS Migration Guide

## Problem
You're getting compilation errors because both the old UPS system and the new simplified UPS system are being compiled together, causing multiple definition errors.

## Solution
You need to choose ONE UPS implementation and remove the other from your project.

## Option 1: Use the New Simplified UPS (Recommended)

### Step 1: Remove Old UPS Files
Delete or exclude these files from your project:
- `ups_core.cpp`
- `ups_core.h`
- `ups_hid.cpp`
- `ups_hid.h`
- `ups_hid_core.cpp`
- `ups_hid_core.h`
- `ups_manager.cpp`
- `ups_manager.h`
- `ups_power_device.cpp`
- `ups_power_device.h`

### Step 2: Update Your Main Code
Replace the old UPS includes and calls:

```cpp
// OLD CODE - Remove this:
#include "ups_manager.h"

void setup() {
    if (setupUPS()) {
        Serial.println("UPS initialized");
    }
}

void loop() {
    loopUPS();
    
    // Access old UPS
    uint16_t capacity = ups.getCapacityPercent();
}
```

```cpp
// NEW CODE - Use this instead:
#include "ups_simple.h"

void setup() {
    if (setupSimpleUPS()) {
        Serial.println("UPS initialized");
    }
}

void loop() {
    loopSimpleUPS();
    
    // Access new UPS
    uint16_t capacity = simple_ups.getCapacityPercent();
}
```

### Step 3: Update Any Other References
Search your code for these old references and update them:
- `ups.getCapacityPercent()` → `simple_ups.getCapacityPercent()`
- `ups.isCharging()` → `simple_ups.isCharging()`
- `ups.getVoltage()` → `simple_ups.getVoltage()`
- `ups.isConnected()` → `simple_ups.isConnected()`

## Option 2: Keep the Old UPS System

### Step 1: Remove New UPS Files
Delete or exclude these files from your project:
- `ups_simple.cpp`
- `ups_simple.h`

### Step 2: Keep Your Existing Code
Your existing code should work as-is with the old system.

## Recommendation

**Use Option 1 (New Simplified UPS)** because:
1. **More Reliable**: Eliminates the crash-causing memory issues
2. **Simpler**: Single file implementation is easier to debug
3. **Better Performance**: Reduced memory usage and faster execution
4. **Future-Proof**: Cleaner architecture for future enhancements

## Quick Migration Checklist

- [ ] Remove old UPS files from project
- [ ] Update `#include` statements
- [ ] Change `setupUPS()` to `setupSimpleUPS()`
- [ ] Change `loopUPS()` to `loopSimpleUPS()`
- [ ] Change `ups.` to `simple_ups.` for all method calls
- [ ] Test compilation
- [ ] Test UPS functionality

## Testing After Migration

1. **Compile**: Ensure no compilation errors
2. **Initialize**: Check UPS initialization in serial monitor
3. **Status**: Verify battery status reporting
4. **LED**: Check status LED behavior
5. **Long-term**: Run for extended periods to ensure stability

## Troubleshooting

### Still Getting Multiple Definition Errors?
- Make sure you've removed ALL old UPS files from the project
- Check that no other files are including the old UPS headers
- Clean and rebuild the project

### UPS Not Working After Migration?
- Check serial output for initialization messages
- Verify I2C connections
- Ensure DFRobot library is properly installed
- Check debug output with `DEBUG_PRINT_UPS 1` in config.h

### Need Help?
- Check the debug output in serial monitor
- Review the simplified architecture documentation
- The new system has much better error reporting
