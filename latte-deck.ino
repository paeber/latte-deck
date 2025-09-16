/*
 * LatteDeck.ino
 *
 * This is the main file for the LatteDeck sketch for LattePanda Delta 3.
 * It provides mouse and keyboard input functionality through gamepad controls.
 */

// Include NicoHood HID library for mouse and keyboard functionality
#include <HID-Project.h>
#include <HID-Settings.h>

#include "usb_config.h"
#include "gamepad.h"
#include "config.h"

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

    setupGamepad();
    Serial.println("Gamepad setup completed");

    Serial.println("LatteDeck ready!");
}

void loop() {
  loopGamepad();
}