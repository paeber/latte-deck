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

//#define DEBUG_PRINT_UPS 1 // Uncomment to enable debug prints
#define DEBUG_PRINT_GAMEPAD 1 // Uncomment to enable debug prints

#define ENABLE_MOUSE_KEYBOARD 1
#define ENABLE_HID_POWER_DEVICE 1

int upsStatus = -1;
int gamepadStatus = -1;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(300); // Give serial time to initialize
    Serial.println("Starting LatteDeck...");
    
    // Initialize USB device configuration
    USBDevice.setManufacturer(LATTE_USB_MANUFACTURER);
    USBDevice.setProduct(LATTE_USB_PRODUCT);
    USBDevice.setSerialNumber(LATTE_USB_SERIAL);
    
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

  #ifdef ENABLE_MOUSE_KEYBOARD
  loopGamepad();
  #endif
}