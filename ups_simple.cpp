#include "ups_simple.h"
#include "DFRobot_LPUPS.h"
#include <Wire.h>

// DFRobot LPUPS Register Definitions
#define CS32_I2C_ADC_VBAT_REG         0x0CU   // VBAT: Full range: 2.88 V - 19.2 V, LSB 64 mV
#define CS32_I2C_ADC_ICHG_REG         0x09U   // ICHG: Full range 8.128 A, LSB 64 mA
#define CS32_I2C_ADC_IDCHG_REG        0x08U   // IDCHG: Full range: 32.512 A, LSB 256 mA

// Battery SoC calculation tables
static const int N = 11;
static const uint16_t SOC_PCT[N] PROGMEM = {  0,   10,  20,  30,  40,  50,  60,  70,  80,  90, 100};
static const uint16_t OCV_mV_A8[N] PROGMEM = {2600,3000,3150,3300,3450,3600,3750,3850,3940,4040,4150};
static const uint16_t OCV_mV_2A[N] PROGMEM = {2600,2900,3070,3220,3370,3520,3670,3780,3900,3980,4100};

// ============================================================================
// Global UPS Instance
// ============================================================================

SimpleUPS simple_ups;

// ============================================================================
// SimpleUPS Class Implementation
// ============================================================================

SimpleUPS::SimpleUPS() : ups_library(nullptr), initialized(false), connected(false), 
                        last_read_ms(0), last_report_ms(0), last_led_update_ms(0),
                        consecutive_failures(0), led_cycle_start_ms(0), led_brightness(0), led_state(false) {
    current_status.voltage_mV = 0;
    current_status.current_mA = 0;
    current_status.capacity_percent = 0;
    current_status.temperature_celsius = 25;
    current_status.is_charging = false;
    current_status.is_connected = false;
    current_status.last_update_ms = 0;
}

SimpleUPS::~SimpleUPS() {
    if (ups_library) {
        delete ups_library;
    }
}

bool SimpleUPS::begin() {
    #if DEBUG_PRINT_UPS
    Serial.println("UPS: Initializing...");
    #endif
    
    // Initialize I2C
    Wire.begin();
    delay(100);
    
    // Create and initialize UPS library
    ups_library = new DFRobot_LPUPS_I2C();
    if (!ups_library) {
        #if DEBUG_PRINT_UPS
        Serial.println("UPS: Library allocation failed");
        #endif
        return false;
    }
    
    int result = ups_library->begin();
    if (result != 0) {
        #if DEBUG_PRINT_UPS
        Serial.print("UPS: Library initialization failed: ");
        Serial.println(result);
        #endif
        delete ups_library;
        ups_library = nullptr;
        return false;
    }
    
    // Set maximum charge voltage for 3-cell battery pack
    ups_library->setMaxChargeVoltage(12600); // 12.6V for 3 cells
    
    // Test communication
    uint8_t regBuf[32];
    if (readRawData(regBuf)) {
        initialized = true;
        connected = true;
        
        // Initialize status LED
        pinMode(UPS_STATUS_LED, OUTPUT);
        digitalWrite(UPS_STATUS_LED, LOW);
        
        #if DEBUG_PRINT_UPS
        Serial.println("UPS: Initialization successful");
        #endif
        return true;
    } else {
        #if DEBUG_PRINT_UPS
        Serial.println("UPS: Communication test failed");
        #endif
        delete ups_library;
        ups_library = nullptr;
        return false;
    }
}

void SimpleUPS::update() {
    if (!initialized) {
        return;
    }
    
    uint32_t current_time = millis();
    
    if (current_time - last_read_ms >= 3000) {
        uint8_t regBuf[32];
        if (readRawData(regBuf)) {
            if (parseBatteryData(regBuf, current_status)) {
                connected = true;
                consecutive_failures = 0;
            } else {
                connected = false;
                consecutive_failures++;
            }
        } else {
            connected = false;
            consecutive_failures++;
        }
        last_read_ms = current_time;
    }
    
    // Update status LED every 50ms for smooth operation
    if (current_time - last_led_update_ms >= 50) {
        updateStatusLED();
        last_led_update_ms = current_time;
    }
    
    // Conservative HID reporting to prevent crashes
    uint32_t reportInterval = 30000; // Default 30 seconds
    if (consecutive_failures > 2) {
        reportInterval = 60000; // 60 seconds if failures
    } else if (consecutive_failures > 0) {
        reportInterval = 45000; // 45 seconds if some failures
    }
    
    // Report battery status at dynamic interval
    if (current_time - last_report_ms >= reportInterval) {
        reportBatteryStatus();
        last_report_ms = current_time;
    }
}

bool SimpleUPS::readRawData(uint8_t* regBuf) {
    if (!ups_library) {
        return false;
    }
    
    // Read chip data from UPS
    ups_library->getChipData(regBuf);
    
    // Check if we have valid battery voltage data
    uint8_t vbat_raw = regBuf[CS32_I2C_ADC_VBAT_REG];
    if (vbat_raw == 0x00) {
        return false; // No valid battery data
    }
    
    return true;
}

bool SimpleUPS::parseBatteryData(const uint8_t* regBuf, SimpleUPSStatus& status) {
    // Extract voltage using DFRobot LPUPS register format
    uint8_t vbat_raw = regBuf[CS32_I2C_ADC_VBAT_REG];
    status.voltage_mV = 2880 + vbat_raw * 64;
    if (status.voltage_mV == 2880) {
        status.voltage_mV = 0; // No battery connected
    }
    
    // Extract charge current
    uint8_t ichg_raw = regBuf[CS32_I2C_ADC_ICHG_REG];
    uint16_t chargeCurrent = ichg_raw * 64;
    
    // Extract discharge current
    uint8_t idchg_raw = regBuf[CS32_I2C_ADC_IDCHG_REG];
    uint16_t dischargeCurrent = idchg_raw * 256;
    
    // Determine if charging or discharging
    if (chargeCurrent > 0) {
        status.current_mA = chargeCurrent;
        status.is_charging = true;
    } else if (dischargeCurrent > 0) {
        status.current_mA = dischargeCurrent;
        status.is_charging = false;
    } else {
        status.current_mA = 0;
        status.is_charging = false;
    }
    
    // Calculate capacity percentage
    status.capacity_percent = calculateSoC(status.voltage_mV, 
                                         status.current_mA > 0 ? status.current_mA : 0,
                                         status.is_charging ? abs(status.current_mA) : 0);
    
    // Clamp capacity to valid range
    if (status.capacity_percent > 100) status.capacity_percent = 100;
    if (status.capacity_percent < 0) status.capacity_percent = 0;
    
    status.is_connected = true;
    status.last_update_ms = millis();
    
    #if DEBUG_PRINT_UPS
    Serial.print("UPS Status - Voltage: ");
    Serial.print(status.voltage_mV);
    Serial.print(" mV, Current: ");
    Serial.print(status.current_mA);
    Serial.print(" mA, Capacity: ");
    Serial.print(status.capacity_percent);
    Serial.print("%, Charging: ");
    Serial.print(status.is_charging ? "Yes" : "No");
    Serial.print(", Connected: ");
    Serial.println(status.is_connected ? "Yes" : "No");
    #endif
    
    return true;
}

uint16_t SimpleUPS::calculateSoC(uint16_t v_pack_mV, uint16_t dischargeCurrent_mA, uint16_t chargeCurrent_mA) {
    uint16_t v_cell_mV = v_pack_mV / N_CELLS_PACK;

    // Compensate for internal resistance sag
    uint16_t v_rest_mV = v_cell_mV;
    
    // Discharge compensation
    if(dischargeCurrent_mA > 100){
      v_rest_mV = v_rest_mV + (dischargeCurrent_mA * R_INTERNAL_mOHM / 1000);
    }

    // Select proper OCV table
    const uint16_t *OCV_V = OCV_mV_A8;
    if (dischargeCurrent_mA > 1200) {
        OCV_V = OCV_mV_2A;
    }

    uint16_t v0 = pgm_read_word(&OCV_V[0]);
    uint16_t v_last = pgm_read_word(&OCV_V[N - 1]);
    if (v_rest_mV <= v0)     return pgm_read_word(&SOC_PCT[0]);
    if (v_rest_mV >= v_last) return pgm_read_word(&SOC_PCT[N - 1]);

    for (int i = 0; i < N - 1; ++i) {
        v0 = pgm_read_word(&OCV_V[i]);
        uint16_t v1 = pgm_read_word(&OCV_V[i + 1]);
        if (v_rest_mV >= v0 && v_rest_mV <= v1) {
            float t = (float)(v_rest_mV - v0) / (float)(v1 - v0);
            uint16_t soc0 = pgm_read_word(&SOC_PCT[i]);
            uint16_t soc1 = pgm_read_word(&SOC_PCT[i + 1]);
            return soc0 + (uint16_t)(t * (soc1 - soc0));
        }
    }

    return pgm_read_word(&SOC_PCT[N - 1]);
}

void SimpleUPS::updateStatusLED() {
    uint32_t current_time = millis();
    
    if (!connected) {
        // Fast blink when disconnected (500ms cycle)
        if (current_time - led_cycle_start_ms >= 500) {
            led_state = !led_state;
            digitalWrite(UPS_STATUS_LED, led_state);
            led_cycle_start_ms = current_time;
        }
        return;
    }
    
    if (current_status.is_charging) {
        // Charging: Fading on and off in 5 second rhythm
        uint32_t cycle_time = (current_time - led_cycle_start_ms) % 5000; // 5 second cycle
        
        if (cycle_time < 2500) {
            // First second: fade in
            led_brightness = map(cycle_time, 0, 2499, 0, 255);
        } else {
            // Second second: fade out
            led_brightness = map(cycle_time, 2500, 4999, 255, 0);
        }
        
        // Apply brightness using PWM
        analogWrite(UPS_STATUS_LED, led_brightness);
        
    } else {
        // Discharging: Blink On/Off in 2 second window while on time represents remaining percentage
        uint32_t cycle_time = (current_time - led_cycle_start_ms) % 5000; // 5 second cycle
        
        // Calculate on-time based on battery percentage (0-100% maps to 0-5000ms)
        uint32_t on_time_ms = map(current_status.capacity_percent, 0, 100, 0, 5000);
        
        if (cycle_time < on_time_ms) {
            // LED should be on
            digitalWrite(UPS_STATUS_LED, HIGH);
        } else {
            // LED should be off
            digitalWrite(UPS_STATUS_LED, LOW);
        }
    }
}

void SimpleUPS::reportBatteryStatus() {
    // Simplified HID reporting - just print to serial for now
    // This eliminates the complex HID implementation that was causing crashes
    #if DEBUG_PRINT_UPS
    Serial.print("UPS Report - Capacity: ");
    Serial.print(current_status.capacity_percent);
    Serial.print("%, Voltage: ");
    Serial.print(current_status.voltage_mV);
    Serial.print("mV, Charging: ");
    Serial.print(current_status.is_charging ? "Yes" : "No");
    Serial.print(", Connected: ");
    Serial.println(current_status.is_connected ? "Yes" : "No");
    #endif
}

// ============================================================================
// Convenience Functions
// ============================================================================

bool setupSimpleUPS() {
    return simple_ups.begin();
}

void loopSimpleUPS() {
    simple_ups.update();
}
