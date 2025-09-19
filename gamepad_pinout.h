
#ifndef GAMEPAD_PINOUT_H
#define GAMEPAD_PINOUT_H

#include <Arduino.h>

// ============================================================================
// Gamepad Pin Configuration
// ============================================================================
// This file defines the hardware pin assignments for the LatteDeck gamepad
// Modify these definitions to match your hardware wiring

// ============================================================================
// Gamepad Configuration Constants
// ============================================================================

// Joystick Inversion Settings
#define JOYSTICK_R_INVERT_X         (1)     // Non-inverted right joystick X-axis
#define JOYSTICK_R_INVERT_Y         (-1)    // Invert right joystick Y-axis
#define JOYSTICK_L_INVERT_X         (-1)    // Invert left joystick X-axis
#define JOYSTICK_L_INVERT_Y         (-1)    // Invert left joystick Y-axis

// Sensitivity and Threshold Settings
#define JOYSTICK_MOUSE_SENSITIVITY  500     // Mouse sensitivity (higher = slower)
#define JOYSTICK_SIDE_MAX           500     // Maximum joystick value for clipping
#define SPRINT_THRESHOLD            480     // Threshold for sprint activation
#define JOYSTICK_X_DEADZONE         10      // Deadzone for X-axis to prevent drift
#define JOYSTICK_Y_DEADZONE         10      // Deadzone for Y-axis to prevent drift
#define JOYSTICK_BINARY_THRESHOLD   100     // Threshold for binary joystick movement

// ============================================================================
// Gamepad Pin Definitions
// ============================================================================

// Gamepad Enable Pin
#define PIN_GAMEPAD_ENABLE          4       // Enable/disable gamepad functionality

// Left Joystick Pins
#define PIN_JOYSTICK_L_X            A5      // Left joystick X-axis (analog)
#define PIN_JOYSTICK_L_Y            A4      // Left joystick Y-axis (analog)
#define PIN_JOYSTICK_L_SEL          21      // Left joystick button (digital)

// Left Button Pins
#define PIN_BTN_L1                  7       // Left shoulder button
#define PIN_BTN_L2                  8       // Left top button
#define PIN_BTN_L3                  11      // Left middle button
#define PIN_BTN_L4                  12      // Left bottom button

// Right Joystick Pins
#define PIN_JOYSTICK_R_X            A2      // Right joystick X-axis (analog)
#define PIN_JOYSTICK_R_Y            A1      // Right joystick Y-axis (analog)
#define PIN_JOYSTICK_R_SEL          18      // Right joystick button (digital)

// Right Button Pins
#define PIN_BTN_R1                  1       // Right shoulder button
#define PIN_BTN_R2                  4       // Right top button (fixed from pin 1)
#define PIN_BTN_R3                  5       // Right middle button
#define PIN_BTN_R4                  6       // Right bottom button

// ============================================================================
// Pin Configuration Summary
// ============================================================================
/*
| Component        | Pin Type | Pin Number | Description                    |
|------------------|----------|------------|--------------------------------|
| Gamepad Enable   | Digital  | 4          | Enable/disable gamepad         |
| Left Joystick    | Analog   | A5         | X-axis movement                |
|                  | Analog   | A4         | Y-axis movement                |
|                  | Digital  | 21         | Button press                   |
| Left Buttons     | Digital  | 7          | L1 (shoulder)                  |
|                  | Digital  | 8          | L2 (top)                       |
|                  | Digital  | 11         | L3 (middle)                    |
|                  | Digital  | 12         | L4 (bottom)                    |
| Right Joystick   | Analog   | A2         | X-axis movement                |
|                  | Analog   | A1         | Y-axis movement                |
|                  | Digital  | 18         | Button press                   |
| Right Buttons    | Digital  | 1          | R1 (shoulder)                  |
|                  | Digital  | 4          | R2 (top)                       |
|                  | Digital  | 5          | R3 (middle)                    |
|                  | Digital  | 6          | R4 (bottom)                    |
*/

// ============================================================================
// Configuration Notes
// ============================================================================
/*
Inversion Settings:
- Set to (-1) to invert axis direction
- Set to (1) for normal axis direction
- Each joystick can have independent X/Y inversion

Mouse Sensitivity:
- Higher values = slower mouse movement
- Lower values = faster mouse movement
- Recommended range: 200-1000

Thresholds:
- SPRINT_THRESHOLD: Joystick magnitude needed for sprint activation
- JOYSTICK_SIDE_MAX: Maximum joystick value before clipping
- Deadzones: Prevent drift when joystick is centered
*/

#endif // GAMEPAD_PINOUT_H