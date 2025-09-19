#ifndef UPS_SIMPLE_H
#define UPS_SIMPLE_H

#include <Arduino.h>
#include "config.h"

// ============================================================================
// Hardware Configuration
// ============================================================================

#define UPS_STATUS_LED              13      // Status LED pin
#define UPS_I2C_ADDRESS             0x55    // I2C address for UPS module

// Battery Configuration
#define N_CELLS_PACK                3       // 3 cells in series
#define MIN_CELL_VOLTAGE            2600    // Minimum cell voltage (mV)
#define MAX_CELL_VOLTAGE            4200    // Maximum cell voltage (mV)
#define NOM_CELL_VOLTAGE            3600    // Nominal cell voltage (mV)
#define CELL_CAPACITY_mAh           4000    // Cell capacity (mAh)
#define R_INTERNAL_mOHM             300     // Internal resistance (mOhm)

#define MIN_BATTERY_VOLTAGE         (N_CELLS_PACK * MIN_CELL_VOLTAGE)
#define MAX_BATTERY_VOLTAGE         (N_CELLS_PACK * MAX_CELL_VOLTAGE)

// ============================================================================
// UPS Status Structure
// ============================================================================

struct SimpleUPSStatus {
    uint16_t voltage_mV;           // Battery voltage in millivolts
    uint16_t current_mA;           // Battery current in milliamps
    uint16_t capacity_percent;     // Battery capacity percentage (0-100)
    uint16_t temperature_celsius;  // Battery temperature in Celsius
    bool is_charging;              // True if battery is charging
    bool is_connected;             // True if UPS is connected and responding
    uint32_t last_update_ms;       // Timestamp of last successful update
};

// ============================================================================
// UPS Class
// ============================================================================
class SimpleUPS {
private:
    // Hardware interface
    class DFRobot_LPUPS_I2C* ups_library;
    
    // State variables
    bool initialized;
    bool connected;
    uint32_t last_read_ms;
    uint32_t last_report_ms;
    uint32_t last_led_update_ms;
    uint8_t consecutive_failures;
    
    // LED control
    uint32_t led_cycle_start_ms;
    uint8_t led_brightness;
    bool led_state;
    
    // Current status
    SimpleUPSStatus current_status;
    uint16_t previous_capacity_percent;  // Track previous capacity for change detection
    
    // Internal methods
    bool readRawData(uint8_t* regBuf);
    bool parseBatteryData(const uint8_t* regBuf, SimpleUPSStatus& status);
    uint16_t calculateSoC(uint16_t v_pack_mV, uint16_t dischargeCurrent_mA, uint16_t chargeCurrent_mA);
    void updateStatusLED();
    void reportBatteryStatus();
    
public:
    SimpleUPS();
    ~SimpleUPS();
    
    // Initialization
    bool begin();
    bool isInitialized() const { return initialized; }
    bool isConnected() const { return connected; }
    
    // Main update loop (non-blocking)
    void update();
    
    // Status access
    const SimpleUPSStatus& getStatus() const { return current_status; }
    uint16_t getCapacityPercent() const { return current_status.capacity_percent; }
    uint16_t getVoltage() const { return current_status.voltage_mV; }
    bool isCharging() const { return current_status.is_charging; }
};

// ============================================================================
// Global Instance
// ============================================================================

extern SimpleUPS simple_ups;

// ============================================================================
// Convenience Functions
// ============================================================================

bool setupSimpleUPS();
void loopSimpleUPS();

#endif // UPS_SIMPLE_H
