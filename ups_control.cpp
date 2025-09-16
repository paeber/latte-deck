#include "ups_control.h"
#include "ups_hid.h"
#include "config.h"
#include <DFRobot_LPUPS.h>

// ============================================================================
// Global UPS Control Instance
// ============================================================================

UPSControl ups;

// ============================================================================
// DFRobot UPS Library Instance
// ============================================================================

DFRobot_LPUPS ups_library;

// ============================================================================
// UPSControl Class Implementation
// ============================================================================

UPSControl::UPSControl() : initialized(false), last_report_ms(0), last_led_update_ms(0), led_state(0) {
    // Initialize status structure
    status.voltage_mV = 0;
    status.current_mA = 0;
    status.capacity_percent = 0;
    status.temperature_celsius = 0;
    status.is_charging = false;
    status.is_connected = false;
    status.last_update_ms = 0;
}

bool UPSControl::begin() {
    printUPS("Initializing UPS...");
    
    // Initialize I2C
    Wire.begin();
    delay(100);
    
    // Initialize UPS library
    int result = ups_library.begin();
    if (result != 0) {
        printUPSF("UPS initialization failed with error: %d", result);
        return false;
    }
    
    // Set maximum charge voltage for 3-cell battery pack
    ups_library.setMaxChargeVoltage(12600); // 12.6V for 3 cells
    
    // Test communication
    uint8_t regBuf[32];
    ups_library.getChipData(regBuf);
    
    // Verify we can read data
    if (readUPSData()) {
        initialized = true;
        status.is_connected = true;
        printUPS("UPS initialization successful");
        printUPSF("Initial voltage: %d mV, Capacity: %d%%", status.voltage_mV, status.capacity_percent);
        return true;
    } else {
        printUPS("UPS communication test failed");
        return false;
    }
}

bool UPSControl::readUPSData() {
    uint8_t regBuf[32];
    
    // Read chip data from UPS
    ups_library.getChipData(regBuf);
    
    // Parse the data according to DFRobot LPUPS library format
    // The exact format may vary - this is a typical implementation
    if (regBuf[0] == 0x00 && regBuf[1] == 0x00) {
        // No valid data
        status.is_connected = false;
        return false;
    }
    
    // Extract voltage (typically in registers 2-3, in mV)
    status.voltage_mV = (regBuf[2] << 8) | regBuf[3];
    
    // Extract current (typically in registers 4-5, in mA)
    status.current_mA = (regBuf[4] << 8) | regBuf[5];
    
    // Extract temperature (typically in register 6, in Celsius)
    status.temperature_celsius = regBuf[6];
    
    // Determine charging status (typically bit in register 7)
    status.is_charging = (regBuf[7] & 0x01) != 0;
    
    // Calculate capacity percentage using our voltage-to-SoC function
    status.capacity_percent = socFromVoltage(status.voltage_mV, 
                                           status.current_mA > 0 ? status.current_mA : 0,
                                           status.is_charging ? abs(status.current_mA) : 0);
    
    // Clamp capacity to valid range
    if (status.capacity_percent > 100) status.capacity_percent = 100;
    if (status.capacity_percent < 0) status.capacity_percent = 0;
    
    status.is_connected = true;
    status.last_update_ms = millis();
    
    return true;
}

void UPSControl::update() {
    if (!initialized) {
        return;
    }
    
    uint32_t current_time = millis();
    
    // Update UPS data every 1 second
    if (current_time - status.last_update_ms >= 1000) {
        if (!readUPSData()) {
            // If we can't read data, mark as disconnected
            status.is_connected = false;
            printUPS("UPS communication lost");
        }
    }
    
    // Update status LED every 500ms
    if (current_time - last_led_update_ms >= 500) {
        updateStatusLED();
        last_led_update_ms = current_time;
    }
    
    // Report battery status via HID every 5 seconds
    if (current_time - last_report_ms >= 5000) {
        reportBatteryStatus();
        last_report_ms = current_time;
    }
}

void UPSControl::updateStatusLED() {
    if (!status.is_connected) {
        // Fast blink when disconnected
        digitalWrite(UPS_STATUS_LED, led_state);
        led_state = !led_state;
        return;
    }
    
    if (status.is_charging) {
        // Solid when charging
        digitalWrite(UPS_STATUS_LED, HIGH);
    } else {
        // Blink rate based on capacity
        if (status.capacity_percent > 50) {
            // Slow blink when > 50%
            digitalWrite(UPS_STATUS_LED, led_state);
            led_state = !led_state;
        } else if (status.capacity_percent > 20) {
            // Medium blink when 20-50%
            digitalWrite(UPS_STATUS_LED, led_state);
            led_state = !led_state;
        } else {
            // Fast blink when < 20%
            digitalWrite(UPS_STATUS_LED, led_state);
            led_state = !led_state;
        }
    }
}

void UPSControl::reportBatteryStatus() {
    // Report battery status via HID Power Device
    ups_hid.reportBatteryStatus(status);
    
    #if DEBUG_PRINT_UPS
    printStatus();
    #endif
}

void UPSControl::printStatus() {
    printUPSF("UPS Status - Voltage: %d mV, Current: %d mA, Capacity: %d%%, Temp: %d°C, Charging: %s, Connected: %s",
              status.voltage_mV,
              status.current_mA,
              status.capacity_percent,
              status.temperature_celsius,
              status.is_charging ? "Yes" : "No",
              status.is_connected ? "Yes" : "No");
}

// ============================================================================
// Global UPS Functions
// ============================================================================

bool setupUPS() {
    // Initialize status LED
    pinMode(UPS_STATUS_LED, OUTPUT);
    digitalWrite(UPS_STATUS_LED, LOW);
    
    // Initialize UPS control
    if (!ups.begin()) {
        return false;
    }
    
    // Initialize HID Power Device
    if (!setupUPSHID()) {
        printUPS("HID Power Device initialization failed");
        return false;
    }
    
    return true;
}

void loopUPS() {
    ups.update();
}

// ============================================================================
// Debug Functions
// ============================================================================

void printUPS(const char* msg) {
    #if DEBUG_PRINT_UPS
    Serial.print("UPS: ");
    Serial.println(msg);
    #endif
}

void printUPS(const String& msg) {
    #if DEBUG_PRINT_UPS
    Serial.print("UPS: ");
    Serial.println(msg);
    #endif
}

void printUPSF(const char* format, ...) {
    #if DEBUG_PRINT_UPS
    Serial.print("UPS: ");
    
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Serial.println(buffer);
    #endif
}
