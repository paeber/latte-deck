#include "ups_hid.h"
#include "config.h"
#include "ups_control.h"

// ============================================================================
// Global HID Power Device Instance
// ============================================================================

UPSHID ups_hid;

// ============================================================================
// UPSHID Class Implementation
// ============================================================================

UPSHID::UPSHID() : initialized(false), last_report_time(0), 
                   last_remaining_capacity(0), last_present_status(0), 
                   last_runtime_to_empty(0) {
}

bool UPSHID::begin() {
    printUPS("Initializing UPS HID Power Device...");
    
    // Initialize UPS Power Device
    UPSPowerDevice.begin();
    
    // Set up string features
    UPSPowerDevice.setSerial("LatteDeck-UPS");
    UPSPowerDevice.setOutput(Serial);
    
    // Set up all required features
    setupPowerDeviceFeatures();
    
    initialized = true;
    printUPS("UPS HID Power Device initialized");
    return true;
}

void UPSHID::setupPowerDeviceFeatures() {
    // String constants
    const char STRING_DEVICE_CHEMISTRY[] PROGMEM = "Li-ion";
    const char STRING_OEM_VENDOR[] PROGMEM = "LatteDeck";
    const char STRING_SERIAL[] PROGMEM = "UPS100";
    
    const byte bDeviceChemistry = IDEVICECHEMISTRY;
    const byte bOEMVendor = IOEMVENDOR;
    
    // Battery parameters
    byte bRechargable = 1;   // Rechargeable Battery
    byte bCapacityMode = 2;   // Battery capacity in percentage
    
    // Physical parameters
    const uint16_t iConfigVoltage = 12600;   // 12.6V nominal for 3-cell UPS
    uint16_t iVoltage = 12600;
    uint16_t iRunTimeToEmpty = 0;
    uint16_t iAvgTimeToFull = 7200;  // 2 hours
    uint16_t iAvgTimeToEmpty = 7200; // 2 hours
    uint16_t iRemainTimeLimit = 600; // 10 minutes
    int16_t iDelayBe4Reboot = -1;
    int16_t iDelayBe4ShutDown = -1;
    byte iAudibleAlarmCtrl = 2; // Enabled
    
    // ACPI parameters
    const byte iDesignCapacity = 100;
    byte iWarnCapacityLimit = 10; // warning at 10%
    byte iRemnCapacityLimit = 5;  // low at 5%
    const byte bCapacityGranularity1 = 1;
    const byte bCapacityGranularity2 = 1;
    byte iFullChargeCapacity = 100;
    
    uint16_t iPresentStatus = 0;
    byte iRemaining = 100;
    
    // Set up all HID Power Device features
    UPSPowerDevice.setFeature(UPS_PD_PRESENTSTATUS, &iPresentStatus, sizeof(iPresentStatus));
    UPSPowerDevice.setFeature(UPS_PD_RUNTIMETOEMPTY, &iRunTimeToEmpty, sizeof(iRunTimeToEmpty));
    UPSPowerDevice.setFeature(UPS_PD_AVERAGETIME2FULL, &iAvgTimeToFull, sizeof(iAvgTimeToFull));
    UPSPowerDevice.setFeature(UPS_PD_AVERAGETIME2EMPTY, &iAvgTimeToEmpty, sizeof(iAvgTimeToEmpty));
    UPSPowerDevice.setFeature(UPS_PD_REMAINTIMELIMIT, &iRemainTimeLimit, sizeof(iRemainTimeLimit));
    UPSPowerDevice.setFeature(UPS_PD_DELAYBE4REBOOT, &iDelayBe4Reboot, sizeof(iDelayBe4Reboot));
    UPSPowerDevice.setFeature(UPS_PD_DELAYBE4SHUTDOWN, &iDelayBe4ShutDown, sizeof(iDelayBe4ShutDown));
    
    UPSPowerDevice.setFeature(UPS_PD_RECHARGEABLE, &bRechargable, sizeof(bRechargable));
    UPSPowerDevice.setFeature(UPS_PD_CAPACITYMODE, &bCapacityMode, sizeof(bCapacityMode));
    UPSPowerDevice.setFeature(UPS_PD_CONFIGVOLTAGE, &iConfigVoltage, sizeof(iConfigVoltage));
    UPSPowerDevice.setFeature(UPS_PD_VOLTAGE, &iVoltage, sizeof(iVoltage));
    
    UPSPowerDevice.setStringFeature(UPS_PD_IDEVICECHEMISTRY, &bDeviceChemistry, STRING_DEVICE_CHEMISTRY);
    UPSPowerDevice.setStringFeature(UPS_PD_IOEMINFORMATION, &bOEMVendor, STRING_OEM_VENDOR);
    
    UPSPowerDevice.setFeature(UPS_PD_AUDIBLEALARMCTRL, &iAudibleAlarmCtrl, sizeof(iAudibleAlarmCtrl));
    
    UPSPowerDevice.setFeature(UPS_PD_DESIGNCAPACITY, &iDesignCapacity, sizeof(iDesignCapacity));
    UPSPowerDevice.setFeature(UPS_PD_FULLCHRGECAPACITY, &iFullChargeCapacity, sizeof(iFullChargeCapacity));
    UPSPowerDevice.setFeature(UPS_PD_REMAININGCAPACITY, &iRemaining, sizeof(iRemaining));
    UPSPowerDevice.setFeature(UPS_PD_WARNCAPACITYLIMIT, &iWarnCapacityLimit, sizeof(iWarnCapacityLimit));
    UPSPowerDevice.setFeature(UPS_PD_REMNCAPACITYLIMIT, &iRemnCapacityLimit, sizeof(iRemnCapacityLimit));
    UPSPowerDevice.setFeature(UPS_PD_CPCTYGRANULARITY1, &bCapacityGranularity1, sizeof(bCapacityGranularity1));
    UPSPowerDevice.setFeature(UPS_PD_CPCTYGRANULARITY2, &bCapacityGranularity2, sizeof(bCapacityGranularity2));
}

void UPSHID::reportBatteryStatus(const UPSStatus& status) {
    if (!initialized) {
        return;
    }
    
    uint32_t current_time = millis();
    bool should_send = false;
    
    // Send report if it's different from the last one or enough time has passed
    if (!last_report_time) {
        should_send = true;
    } else if (current_time - last_report_time >= 5000) { // Send every 5 seconds
        should_send = true;
    } else if (status.capacity_percent != last_remaining_capacity) {
        should_send = true;
    }
    
    if (should_send) {
        // Update present status
        updatePresentStatus(status);
        
        // Send remaining capacity (required by Windows)
        byte remaining = (byte)status.capacity_percent;
        UPSPowerDevice.sendReport(UPS_PD_REMAININGCAPACITY, &remaining, sizeof(remaining));
        
        // Send runtime to empty if discharging
        if (status.is_charging == false && status.current_mA > 0) {
            uint16_t runtime_to_empty = (uint16_t)((float)7200 * status.capacity_percent / 100.0);
            UPSPowerDevice.sendReport(UPS_PD_RUNTIMETOEMPTY, &runtime_to_empty, sizeof(runtime_to_empty));
        }
        
        // Send present status
        UPSPowerDevice.sendReport(UPS_PD_PRESENTSTATUS, &last_present_status, sizeof(last_present_status));
        
        // Update tracking variables
        last_report_time = current_time;
        last_remaining_capacity = status.capacity_percent;
        last_runtime_to_empty = (uint16_t)((float)7200 * status.capacity_percent / 100.0);
        
        #if DEBUG_PRINT_UPS
        printStatus(status);
        #endif
    }
}

void UPSHID::updatePresentStatus(const UPSStatus& status) {
    last_present_status = 0;
    
    // Set battery present
    bitSet(last_present_status, PRESENTSTATUS_BATTPRESENT);
    
    // Set charging status
    if (status.is_charging) {
        bitSet(last_present_status, PRESENTSTATUS_CHARGING);
    } else {
        bitSet(last_present_status, PRESENTSTATUS_DISCHARGING);
    }
    
    // Set AC present (assume AC is present if charging)
    if (status.is_charging) {
        bitSet(last_present_status, PRESENTSTATUS_ACPRESENT);
    }
    
    // Set fully charged
    if (status.capacity_percent >= 100 && status.is_charging) {
        bitSet(last_present_status, PRESENTSTATUS_FULLCHARGE);
    }
    
    // Set critical battery
    if (status.capacity_percent <= 5) {
        bitSet(last_present_status, PRESENTSTATUS_BELOWRCL);
    }
}

void UPSHID::printStatus(const UPSStatus& status) {
    Serial.print("UPS HID Report - Voltage: ");
    Serial.print(status.voltage_mV);
    Serial.print(" mV, Current: ");
    Serial.print(status.current_mA);
    Serial.print(" mA, Capacity: ");
    Serial.print(status.capacity_percent);
    Serial.print("%, Charging: ");
    Serial.print(status.is_charging ? "Yes" : "No");
    Serial.print(", Present Status: 0x");
    Serial.print(last_present_status, HEX);
    Serial.println();
}

// ============================================================================
// Global Functions
// ============================================================================

bool setupUPSHID() {
    return ups_hid.begin();
}

void reportUPSBatteryStatus() {
    // This function is called from the main UPS control loop
    // The actual reporting is handled by the UPS control system
}