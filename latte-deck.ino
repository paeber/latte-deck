/*
 * LatteDeck.ino
 *
 * This is the main file for the LatteDeck sketch for LattePanda Delta 3.
 * It provides mouse and keyboard input functionality through gamepad controls.
 */

#include "config.h"
#include "usb_config.h"
#include "gamepad.h"
#include "ups_simple.h"

int gamepadStatus = -1;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(3000); // Give serial time to initialize
    Serial.println("Starting LatteDeck...");
    
    // USB configuration is handled through Arduino IDE board settings
    // and the USB_VID and USB_PID definitions in usb_config.h
    // The Leonardo doesn't support runtime USB descriptor changes
    
    // Initialize NicoHood HID for mouse and keyboard functionality
    Mouse.begin();
    Keyboard.begin();
    Serial.println("NicoHood HID initialized");

    setupGamepad();
    Serial.println("Gamepad setup completed");

    // Initialize UPS functionality
    #if ENABLE_HID_POWER_DEVICE
    if (setupSimpleUPS()) {
        Serial.println("UPS setup completed");
    } else {
        Serial.println("UPS setup failed - continuing without UPS");
    }
    #endif

    Serial.println("LatteDeck ready!");
}

void loop() {
  loopGamepad();
  
  // Update UPS functionality (non-blocking)
  #if ENABLE_HID_POWER_DEVICE
  loopSimpleUPS();
  #endif
}
