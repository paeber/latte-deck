#ifndef UPS_CTRL_H
#define UPS_CTRL_H

#include <Arduino.h>
#include "upsDef.h"

// Initializes UPS (I2C device, LEDs, HID power device)
int setupUPS(void);

// Main UPS loop handler (to be called from Arduino loop or scheduler)
void loopUPS(void);

// Estimate SoC (%) from pack voltage and charge/discharge currents
uint16_t socFromVoltage(uint16_t v_pack_mV,
                        uint16_t dischargeCurrent_mA,
                        uint16_t chargeCurrent_mA);

// Add a reading into the moving average filter and return the averaged value
int addMovingAVG(int value);

// Function to set the color of the RGB LED using PWM
void setRGBColor(uint8_t red, uint8_t green, uint8_t blue);

// Function to initialize the RGB LED pins
void initRGBLed();

// Function to set the RGB LED color with optional breathing effect
void setRGBColorWithEffect(uint8_t red, uint8_t green, uint8_t blue, bool pulse);

#endif //UPS_CTRL_H