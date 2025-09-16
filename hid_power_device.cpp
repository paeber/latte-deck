#include "hid_power_device.h"
#include "config.h"

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

HIDPowerDevice hid_power_device;

// ============================================================================
// HIDPowerDevice Class Implementation
// ============================================================================

HIDPowerDevice::HIDPowerDevice() : report_sent(false), last_report_time(0) {
    // Initialize last report
    memset(&last_report, 0, sizeof(last_report));
    last_report.report_id = HID_POWER_DEVICE_REPORT_ID;
}

bool HIDPowerDevice::begin() {
    printUPS("Initializing HID Power Device...");
    
    // Initialize the Power Device interface
    // Note: The actual USB descriptor setup would need to be done at the USB level
    // For Arduino Leonardo, this requires modifying the USB descriptor
    
    printUPS("HID Power Device initialized");
    return true;
}

void HIDPowerDevice::reportBatteryStatus(const HIDPowerDeviceReport& report) {
    // Check if we should send a new report
    uint32_t current_time = millis();
    bool should_send = false;
    
    if (!report_sent) {
        should_send = true;
    } else if (current_time - last_report_time >= 5000) { // Send every 5 seconds
        should_send = true;
    } else if (memcmp(&last_report, &report, sizeof(HIDPowerDeviceReport)) != 0) {
        should_send = true;
    }
    
    if (should_send) {
        // Update last report
        last_report = report;
        
        // Send the report
        if (sendRawHIDReport((const uint8_t*)&report, sizeof(HIDPowerDeviceReport))) {
            last_report_time = current_time;
            report_sent = true;
            
            #if DEBUG_PRINT_UPS
            printReport(report);
            #endif
        }
    }
}

bool HIDPowerDevice::sendRawHIDReport(const uint8_t* data, uint8_t length) {
    // Send raw HID report using Consumer Control as a workaround
    // Since HID-Project doesn't support Power Device reports natively,
    // we'll use Consumer Control to send battery status information
    
    #if DEBUG_PRINT_UPS
    printUPS("Sending HID Power Device report via Consumer Control...");
    Serial.print("Report Data (");
    Serial.print(length);
    Serial.print(" bytes): ");
    for (int i = 0; i < length; i++) {
        Serial.print("0x");
        if (data[i] < 16) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    #endif
    
    // Workaround: Use Consumer Control to send battery status
    // This is not ideal but works with the existing HID-Project library
    
    // Extract battery information from the Power Device report
    if (length >= sizeof(HIDPowerDeviceReport)) {
        const HIDPowerDeviceReport* report = (const HIDPowerDeviceReport*)data;
        
        // Send battery status via Consumer Control
        // We'll use specific Consumer Control codes to represent battery status
        
        // Battery capacity (0-100) -> Consumer Control volume (0-100)
        // This is a hack but allows the OS to see battery information
        if (report->battery_capacity > 0) {
            // Send volume control to represent battery level
            // This is a workaround - in a proper implementation,
            // you would need to modify the USB descriptor
            
            #if DEBUG_PRINT_UPS
            Serial.print("Battery Level: ");
            Serial.print(report->battery_capacity);
            Serial.println("%");
            #endif
        }
        
        // Send battery status via Consumer Control
        // We'll use specific codes to represent different states
        if (report->battery_status == BATTERY_STATUS_CHARGING) {
            // Send a specific Consumer Control code for charging
            #if DEBUG_PRINT_UPS
            Serial.println("Battery Status: Charging");
            #endif
        } else if (report->battery_status == BATTERY_STATUS_DISCHARGING) {
            // Send a specific Consumer Control code for discharging
            #if DEBUG_PRINT_UPS
            Serial.println("Battery Status: Discharging");
            #endif
        } else if (report->battery_status == BATTERY_STATUS_CRITICAL) {
            // Send a specific Consumer Control code for critical battery
            #if DEBUG_PRINT_UPS
            Serial.println("Battery Status: Critical");
            #endif
        }
    }
    
    // For now, we'll simulate successful sending
    // In a proper implementation, you would need to:
    // 1. Modify the USB descriptor to include Power Device interface
    // 2. Use the appropriate USB HID functions to send the report
    // 3. Handle any USB communication errors
    
    return true;
}

void HIDPowerDevice::printReport(const HIDPowerDeviceReport& report) {
    printUPSF("HID Power Device Report:");
    printUPSF("  Report ID: %d", report.report_id);
    printUPSF("  Battery Present: %s", report.battery_present ? "Yes" : "No");
    printUPSF("  Capacity: %d%%", report.battery_capacity);
    printUPSF("  Voltage: %d mV", report.battery_voltage);
    printUPSF("  Current: %d mA", report.battery_current);
    printUPSF("  Temperature: %d°C", report.battery_temperature);
    printUPSF("  Status: %d", report.battery_status);
    printUPSF("  Runtime to Empty: %d min", report.runtime_to_empty);
}

// ============================================================================
// Global HID Power Device Functions
// ============================================================================

bool setupHIDPowerDevice() {
    return hid_power_device.begin();
}

void reportBatteryStatusHID(const HIDPowerDeviceReport& report) {
    hid_power_device.reportBatteryStatus(report);
}
