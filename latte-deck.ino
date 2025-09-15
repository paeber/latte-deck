/*
 * LatteDeck.ino
 *
 * This is the main file for the LatteDeck sketch for LattePanda Delta 3.
 * It provides a composite HID device with UPS battery reporting, mouse, and keyboard functionality.
 */

// CRITICAL: Include NicoHood HID library FIRST to ensure it takes priority
// over any HID functionality in the DFRobot LPUPS library
#include <HID-Project.h>
#include <HID-Settings.h>

// Include USB configuration for proper device recognition
#include "usb_config.h"

// Include our custom composite HID system
#include "composite_hid.h"

// Include UPS and gamepad modules (these may include DFRobot library)
#include "ups_ctrl.h"
#include "gamepad.h"

// Include configuration
#include "config.h"

int upsStatus = -1;
int gamepadStatus = -1;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(300); // Give serial time to initialize
    Serial.println("Starting LatteDeck...");
    
    // USB configuration is handled through Arduino IDE board settings
    // and the USB_VID and USB_PID definitions in usb_config.h
    // The Leonardo doesn't support runtime USB descriptor changes
    
    // Initialize the composite HID device first
    CompositeHID::begin();
    Serial.println("Composite HID initialized");

    #if ENABLE_HID_POWER_DEVICE
    upsStatus = setupUPS();
    if (upsStatus != 0) {
        Serial.println("UPS setup failed, continuing without UPS functionality.");
    } else {
        Serial.println("UPS setup successful");
    }
    #endif

    #if ENABLE_MOUSE_KEYBOARD
    setupGamepad();
    Serial.println("Gamepad setup completed");
    #endif

    Serial.println("LatteDeck ready!");
}

void loop() {
  #if ENABLE_HID_POWER_DEVICE
  if (upsStatus == 0) {
    loopUPS();
  }
  #endif

  #if ENABLE_MOUSE_KEYBOARD
  loopGamepad();
  #endif
}