#include "ups_hid.h"
#include "config.h"
#include "hid_descriptor.h"
#include "hid_leonardo.h"

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

UPSHID ups_hid;

// ============================================================================
// UPSHID Class Implementation
// ============================================================================

UPSHID::UPSHID() {
    // Initialize last report
    memset(&last_report, 0, sizeof(last_report));
    last_report.report_id = UPS_HID_REPORT_ID;
}

bool UPSHID::begin() {
    printUPS("Initializing UPS HID interface...");
    
    // Initialize the Leonardo HID interface
    if (!setupLeonardoHID()) {
        printUPS("Failed to initialize Leonardo HID");
        return false;
    }
    
    printUPS("UPS HID interface initialized");
    return true;
}

void UPSHID::reportBatteryStatus(const UPSStatus& status) {
    // Create new report
    createReport(status);
    
    // Convert to Leonardo HID report format
    LeonardoHIDPowerDeviceReport leonardo_report;
    leonardo_report.report_id = last_report.report_id;
    leonardo_report.battery_present = last_report.battery_present;
    leonardo_report.battery_capacity = last_report.battery_capacity;
    leonardo_report.battery_voltage = last_report.battery_voltage;
    leonardo_report.battery_current = last_report.battery_current;
    leonardo_report.battery_temperature = last_report.battery_temperature;
    leonardo_report.battery_status = last_report.battery_status;
    leonardo_report.runtime_to_empty = last_report.runtime_to_empty;
    
    // Send the report using the Leonardo HID interface
    sendBatteryStatusLeonardo(leonardo_report);
}

void UPSHID::createReport(const UPSStatus& status) {
    // Create HID Power Device report
    last_report.report_id = UPS_HID_REPORT_ID;
    last_report.battery_present = status.is_connected ? 1 : 0;
    last_report.battery_capacity = status.capacity_percent;
    last_report.battery_voltage = status.voltage_mV;
    
    // Debug: Print HID report values and raw data
    #if DEBUG_PRINT_UPS
    Serial.print("HID Report - Voltage: ");
    Serial.print(status.voltage_mV);
    Serial.print(" mV, Current: ");
    Serial.print(status.current_mA);
    Serial.print(" mA, Capacity: ");
    Serial.print(status.capacity_percent);
    Serial.print("%, Charging: ");
    Serial.print(status.is_charging ? "Yes" : "No");
    Serial.println();
    
    // Print raw UPS data
    Serial.print("UPS Raw Data: ");
    for (int i = 0; i < 16; i++) {
        Serial.print("0x");
        if (status.raw_data[i] < 16) Serial.print("0");
        Serial.print(status.raw_data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    #endif
    
    // Current: positive for charging, negative for discharging
    if (status.is_charging) {
        last_report.battery_current = abs(status.current_mA);
    } else {
        last_report.battery_current = -abs(status.current_mA);
    }
    
    last_report.battery_temperature = status.temperature_celsius;
    
    // Determine battery status
    if (!status.is_connected) {
        last_report.battery_status = BATTERY_STATUS_UNKNOWN;
    } else if (status.capacity_percent >= 100 && status.is_charging) {
        last_report.battery_status = BATTERY_STATUS_FULLY_CHARGED;
    } else if (status.is_charging) {
        last_report.battery_status = BATTERY_STATUS_CHARGING;
    } else if (status.capacity_percent <= 10) {
        last_report.battery_status = BATTERY_STATUS_CRITICAL;
    } else {
        last_report.battery_status = BATTERY_STATUS_DISCHARGING;
    }
    
    // Calculate estimated runtime to empty (rough estimation)
    if (status.current_mA > 0 && !status.is_charging) {
        // Runtime = (capacity_mAh / current_mA) * 60 minutes
        // Note: Cells in series have same capacity as single cell (voltage adds, capacity doesn't)
        uint16_t capacity_mAh = (status.capacity_percent * CELL_CAPACITY_mAh) / 100;
        last_report.runtime_to_empty = (capacity_mAh * 60) / status.current_mA;
        
        // Debug: Print runtime calculation
        #if DEBUG_PRINT_UPS
        Serial.print("Runtime Calc - Capacity: ");
        Serial.print(capacity_mAh);
        Serial.print(" mAh, Current: ");
        Serial.print(status.current_mA);
        Serial.print(" mA, Runtime: ");
        Serial.print(last_report.runtime_to_empty);
        Serial.println(" min");
        #endif
    } else {
        last_report.runtime_to_empty = 0;
    }
}


void UPSHID::printReport(const HIDPowerDeviceReport& report) {
    printUPSF("HID Report - Present: %d, Capacity: %d%%, Voltage: %d mV, Current: %d mA, Temp: %d°C, Status: %d, Runtime: %d min",
              report.battery_present,
              report.battery_capacity,
              report.battery_voltage,
              report.battery_current,
              report.battery_temperature,
              report.battery_status,
              report.runtime_to_empty);
}

// ============================================================================
// Global HID Power Device Functions
// ============================================================================

bool setupUPSHID() {
    return ups_hid.begin();
}

void reportUPSBatteryStatus() {
    if (ups.isInitialized()) {
        const UPSStatus& status = ups.getStatus();
        ups_hid.reportBatteryStatus(status);
    }
}
