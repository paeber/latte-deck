#include "ups_hid.h"
#include "config.h"

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

UPSHID ups_hid;

// ============================================================================
// UPSHID Class Implementation
// ============================================================================

UPSHID::UPSHID() : report_sent(false), last_report_time(0) {
    // Initialize last report
    memset(&last_report, 0, sizeof(last_report));
    last_report.report_id = UPS_HID_REPORT_ID;
}

bool UPSHID::begin() {
    printUPS("Initializing HID Power Device...");
    
    // Initialize HID Power Device
    // Note: The actual HID descriptor setup is handled by the HID-Project library
    // and the hid_config.h configuration
    
    printUPS("HID Power Device initialized");
    return true;
}

void UPSHID::reportBatteryStatus(const UPSStatus& status) {
    // Create new report
    createReport(status);
    
    // Send report if it's different from the last one or enough time has passed
    uint32_t current_time = millis();
    bool should_send = false;
    
    if (!report_sent) {
        should_send = true;
    } else if (current_time - last_report_time >= 5000) { // Send every 5 seconds
        should_send = true;
    } else if (memcmp(&last_report, &last_report, sizeof(HIDPowerDeviceReport)) != 0) {
        should_send = true;
    }
    
    if (should_send) {
        if (sendReport()) {
            last_report_time = current_time;
            report_sent = true;
            
            #if DEBUG_PRINT_UPS
            printReport(last_report);
            #endif
        }
    }
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

bool UPSHID::sendReport() {
    // Send HID Power Device report
    // Note: This is a simplified implementation. In a real implementation,
    // you would need to properly configure the HID descriptor and use
    // the appropriate HID library functions.
    
    // For now, we'll use a basic approach with the HID-Project library
    // The actual implementation may need to be adjusted based on the
    // specific HID library being used and the target operating system.
    
    #if DEBUG_PRINT_UPS
    printUPS("Sending HID Power Device report...");
    #endif
    
    // This is a placeholder - the actual HID report sending would depend
    // on the specific HID library implementation and descriptor setup
    // For now, we'll just indicate that the report was "sent"
    return true;
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
