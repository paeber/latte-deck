/*
  UPS Simple Example
  
  This example shows how to use the new simplified UPS implementation.
  It replaces the complex multi-file UPS system with a single, reliable implementation.
  
  Hardware:
  - DFRobot LPUPS module connected via I2C
  - Status LED on pin 13
  
  Features:
  - Battery voltage and current monitoring
  - State of Charge (SoC) calculation
  - Status LED with charging/discharging patterns
  - Serial output for debugging
  - Non-blocking operation
*/

#include "ups_simple.h"

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("LatteDeck UPS Simple Example");
  Serial.println("=============================");
  
  // Initialize UPS
  if (setupSimpleUPS()) {
    Serial.println("✓ UPS initialized successfully");
  } else {
    Serial.println("✗ UPS initialization failed");
    Serial.println("Check I2C connections and UPS module");
  }
  
  Serial.println("\nUPS Status will be reported every 30 seconds");
  Serial.println("LED patterns:");
  Serial.println("- Charging: Fading in/out every 2 seconds");
  Serial.println("- Discharging: Blink pattern (on-time = battery %)");
  Serial.println("- Disconnected: Fast blink (500ms cycle)");
  Serial.println("\nStarting main loop...\n");
}

void loop() {
  // Update UPS (non-blocking)
  loopSimpleUPS();
  
  // Check UPS status
  if (simple_ups.isInitialized() && simple_ups.isConnected()) {
    // Get current status
    const SimpleUPSStatus& status = simple_ups.getStatus();
    
    // Print detailed status every 10 seconds
    static uint32_t last_detail_report = 0;
    if (millis() - last_detail_report >= 10000) {
      Serial.println("=== Detailed UPS Status ===");
      Serial.print("Voltage: ");
      Serial.print(status.voltage_mV);
      Serial.println(" mV");
      
      Serial.print("Current: ");
      Serial.print(status.current_mA);
      Serial.println(" mA");
      
      Serial.print("Capacity: ");
      Serial.print(status.capacity_percent);
      Serial.println(" %");
      
      Serial.print("Temperature: ");
      Serial.print(status.temperature_celsius);
      Serial.println(" °C");
      
      Serial.print("Charging: ");
      Serial.println(status.is_charging ? "Yes" : "No");
      
      Serial.print("Connected: ");
      Serial.println(status.is_connected ? "Yes" : "No");
      
      Serial.print("Last Update: ");
      Serial.print(millis() - status.last_update_ms);
      Serial.println(" ms ago");
      
      Serial.println("========================\n");
      
      last_detail_report = millis();
    }
    
    // Example: Take action based on battery level
    if (status.capacity_percent < 20 && !status.is_charging) {
      // Low battery warning (only print once)
      static bool low_battery_warning = false;
      if (!low_battery_warning) {
        Serial.println("⚠️  WARNING: Low battery! Connect charger.");
        low_battery_warning = true;
      }
    } else if (status.capacity_percent > 30) {
      // Reset warning when battery is above 30%
      static bool low_battery_warning = false;
      low_battery_warning = false;
    }
    
  } else if (simple_ups.isInitialized()) {
    // UPS initialized but not connected
    static uint32_t last_connection_warning = 0;
    if (millis() - last_connection_warning >= 5000) {
      Serial.println("⚠️  UPS module not responding - check connections");
      last_connection_warning = millis();
    }
  }
  
  // Your other application code goes here
  // The UPS update is non-blocking, so you can do other tasks
  
  // Small delay to prevent overwhelming the serial output
  delay(100);
}

/*
  Expected Serial Output:
  
  LatteDeck UPS Simple Example
  =============================
  ✓ UPS initialized successfully
  
  UPS Status will be reported every 30 seconds
  LED patterns:
  - Charging: Fading in/out every 2 seconds
  - Discharging: Blink pattern (on-time = battery %)
  - Disconnected: Fast blink (500ms cycle)
  
  Starting main loop...
  
  UPS Status - Voltage: 12600 mV, Current: 500 mA, Capacity: 85%, Charging: Yes, Connected: Yes
  === Detailed UPS Status ===
  Voltage: 12600 mV
  Current: 500 mA
  Capacity: 85 %
  Temperature: 25 °C
  Charging: Yes
  Connected: Yes
  Last Update: 0 ms ago
  ========================
  
  UPS Report - Capacity: 85%, Voltage: 12600mV, Charging: Yes, Connected: Yes
*/
