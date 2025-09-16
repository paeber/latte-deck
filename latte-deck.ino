/*
 * LatteDeck.ino
 *
 * This is the main file for the LatteDeck sketch for LattePanda Delta 3.
 * It provides a composite HID device with UPS battery reporting, mouse, and keyboard functionality.
 */

// Include HID configuration first to set up Report IDs
#include "hid_config.h"

// Include NicoHood-compatible DFRobot LPUPS library for battery reporting
#include "DFRobot_LPUPS_NicoHood.h"

// Include NicoHood HID library for mouse and keyboard functionality
#include <HID-Project.h>
#include <HID-Settings.h>

#include "usb_config.h"
#include "ups_ctrl.h"
#include "ups_hid.h"
#include "gamepad.h"

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
    
    // Initialize NicoHood HID for mouse and keyboard functionality
    Mouse.begin();
    Keyboard.begin();
    Serial.println("NicoHood HID initialized");
    
    // Initialize UPS HID for battery reporting
    UPSHID::begin();
    Serial.println("UPS HID initialized");


    upsStatus = setupUPS();
    if (upsStatus != 0) {
        Serial.println("UPS setup failed, continuing without UPS functionality.");
    } else {
        Serial.println("UPS setup successful");
    }


    setupGamepad();
    Serial.println("Gamepad setup completed");

    Serial.println("LatteDeck ready!");
}

static int loopCount = 0;
void loop() {
  if (loopCount % 100 == 0) {
    if (upsStatus == 0) {
      loopUPS();
    }
  }

  loopGamepad();

  loopCount++;
}