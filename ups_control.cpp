#include "ups_control.h"
#include "ups_hid.h"
#include "config.h"
#include <DFRobot_LPUPS.h>
#include "ups.h"

// DFRobot LPUPS Register Definitions
#define CS32_I2C_ADC_VBAT_REG         0x0CU   // VBAT: Full range: 2.88 V - 19.2 V, LSB 64 mV
#define CS32_I2C_ADC_ICHG_REG         0x09U   // ICHG: Full range 8.128 A, LSB 64 mA
#define CS32_I2C_ADC_IDCHG_REG        0x08U   // IDCHG: Full range: 32.512 A, LSB 256 mA

// ============================================================================
// Global UPS Control Instance
// ============================================================================

UPSControl ups;

// ============================================================================
// DFRobot UPS Library Instance
// ============================================================================

DFRobot_LPUPS_I2C ups_library;

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
    
    // Store raw data for debug output in HID report
    memcpy(status.raw_data, regBuf, 16);
    
    // Parse the data according to DFRobot LPUPS library format
    // Check if we have valid battery voltage data (VBAT register should not be 0)
    // Note: When on battery power, charger status registers (0x00, 0x01) may be 0x00 0x00
    // but battery voltage data is still valid
    uint8_t vbat_raw = regBuf[CS32_I2C_ADC_VBAT_REG];
    if (vbat_raw == 0x00) {
        // No valid battery data - likely communication error
        status.is_connected = false;
        return false;
    }
    
    // Extract voltage using correct DFRobot LPUPS register format
    // VBAT: Full range: 2.88 V - 19.2 V, LSB 64 mV
    // Formula: 2880 + regBuf[CS32_I2C_ADC_VBAT_REG] * 64
    // Note: vbat_raw already declared above
    status.voltage_mV = 2880 + vbat_raw * 64;
    if (status.voltage_mV == 2880) {
        status.voltage_mV = 0; // No battery connected
    }
    
    // Extract charge current (ICHG: Full range 8.128 A, LSB 64 mA)
    uint8_t ichg_raw = regBuf[CS32_I2C_ADC_ICHG_REG];
    uint16_t chargeCurrent = ichg_raw * 64;
    
    // Extract discharge current (IDCHG: Full range: 32.512 A, LSB 256 mA)
    uint8_t idchg_raw = regBuf[CS32_I2C_ADC_IDCHG_REG];
    uint16_t dischargeCurrent = idchg_raw * 256;
    
    // Debug: Print parsed values
    #if DEBUG_PRINT_UPS
    Serial.print("UPS Parsed - VBAT raw: 0x");
    Serial.print(vbat_raw, HEX);
    Serial.print(", Voltage: ");
    Serial.print(status.voltage_mV);
    Serial.print(" mV, ICHG raw: 0x");
    Serial.print(ichg_raw, HEX);
    Serial.print(", IDCHG raw: 0x");
    Serial.print(idchg_raw, HEX);
    Serial.print(", Charge: ");
    Serial.print(chargeCurrent);
    Serial.print(" mA, Discharge: ");
    Serial.print(dischargeCurrent);
    Serial.print(" mA, Charger Status: 0x");
    Serial.print(regBuf[0], HEX);
    Serial.print(" 0x");
    Serial.print(regBuf[1], HEX);
    Serial.println();
    #endif
    
    // Determine if charging or discharging
    if (chargeCurrent > 0) {
        status.current_mA = chargeCurrent;
        status.is_charging = true;
    } else if (dischargeCurrent > 0) {
        status.current_mA = dischargeCurrent;
        status.is_charging = false;
    } else {
        // No current detected, but if we're on battery power (no AC), 
        // we might still be discharging at a low rate
        status.current_mA = 0;
        status.is_charging = false;
    }
    
    // Extract temperature (not directly available in LPUPS, use default)
    status.temperature_celsius = 25; // Default room temperature
    
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
    
    // Update status LED every 50ms for smooth fading
    if (current_time - last_led_update_ms >= 50) {
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
    uint32_t current_time = millis();
    
    if (!status.is_connected) {
        // Fast blink when disconnected (500ms cycle)
        if (current_time - led_cycle_start_ms >= 500) {
            led_state = !led_state;
        digitalWrite(UPS_STATUS_LED, led_state);
            led_cycle_start_ms = current_time;
        }
        return;
    }
    
    if (status.is_charging) {
        // Charging: Fading on and off in 2 second rhythm
        uint32_t cycle_time = (current_time - led_cycle_start_ms) % 2000; // 2 second cycle
        
        if (cycle_time < 1000) {
            // First second: fade in
            led_brightness = map(cycle_time, 0, 999, 0, 255);
        } else {
            // Second second: fade out
            led_brightness = map(cycle_time, 1000, 1999, 255, 0);
        }
        
        // Apply brightness using PWM (Arduino Leonardo supports PWM on pin 13)
        analogWrite(UPS_STATUS_LED, led_brightness);
        
    } else {
        // Discharging: Blink On/Off in 2 second window while on time represents remaining percentage
        uint32_t cycle_time = (current_time - led_cycle_start_ms) % 2000; // 2 second cycle
        
        // Calculate on-time based on battery percentage (0-100% maps to 0-2000ms)
        uint32_t on_time_ms = map(status.capacity_percent, 0, 100, 0, 2000);
        
        if (cycle_time < on_time_ms) {
            // LED should be on
            digitalWrite(UPS_STATUS_LED, HIGH);
        } else {
            // LED should be off
            digitalWrite(UPS_STATUS_LED, LOW);
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
