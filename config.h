#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#include <HID-Project.h>
#include <HID-Settings.h>

// ============================================================================
// Hardware Configuration
// ============================================================================

// ============================================================================
// Gamepad Pin Definitions
// ============================================================================

// Gamepad Enable
#define PIN_GAMEPAD_ENABLE          4

// Left Joystick
#define PIN_JOYSTICK_L_X            A5
#define PIN_JOYSTICK_L_Y            A4
#define PIN_JOYSTICK_L_SEL          21

// Left Buttons
#define PIN_BTN_L1                  7       // Shoulder button
#define PIN_BTN_L2                  8       // Top button
#define PIN_BTN_L3                  11      // Middle button
#define PIN_BTN_L4                  12      // Bottom button

// Right Joystick
#define PIN_JOYSTICK_R_X            A2
#define PIN_JOYSTICK_R_Y            A1
#define PIN_JOYSTICK_R_SEL          18

// Right Buttons
#define PIN_BTN_R1                  1       // Shoulder button
#define PIN_BTN_R2                  4       // Top button (fixed from pin 1)
#define PIN_BTN_R3                  5       // Middle button
#define PIN_BTN_R4                  6       // Bottom button

// ============================================================================
// Gamepad Configuration
// ============================================================================

#define JOYSTICK_SIDE_MAX           500
#define SPRINT_THRESHOLD            480
#define JOYSTICK_X_DEADZONE         20
#define JOYSTICK_Y_DEADZONE         20

// ============================================================================
// Timing Configuration
// ============================================================================

#define MIN_UPDATE_INTERVAL         26      // Minimum update interval for USB-HID
#define MIN_SERIAL_REPORT_INTERVAL  5000    // Minimum interval for serial output

// ============================================================================
// Feature Enable Flags
// ============================================================================

// Debug configuration - uncomment to enable
#define DEBUG_PRINT_GAMEPAD 1

// ============================================================================
// Memory Configuration
// ============================================================================

#define DATA_LEN_MAX                0x24U
#define OUTPUT_BUFFER_SIZE          256     // Reduced from 512 to save memory
#define DEBUG_BUFFER_SIZE           80      // Reduced from 96 to save memory

// ============================================================================
// Action Assignments
// ============================================================================

#define BUTTON_UNDEFINED            0

// Left Joystick Actions
#define ACTION_JOYSTICK_L_UP        KEY_W
#define ACTION_JOYSTICK_L_DOWN      KEY_S
#define ACTION_JOYSTICK_L_LEFT      KEY_A
#define ACTION_JOYSTICK_L_RIGHT     KEY_D
#define ACTION_JOYSTICK_L_PRESS     KEY_SPACE   // Space key
#define ACTION_JOYSTICK_L_MAX       KEY_E       // Sprint when moving joystick beyond threshold

// Left Button Actions
#define ACTION_BTN_L1               MOUSE_RIGHT
#define ACTION_BTN_L2               KEY_Q       // Top button
#define ACTION_BTN_L3               KEY_1       // Middle button
#define ACTION_BTN_L4               KEY_2       // Bottom button

// Right Joystick Actions (Mouse)
#define ACTION_JOYSTICK_R_UP        MOUSE_MOVE_UP
#define ACTION_JOYSTICK_R_DOWN      MOUSE_MOVE_DOWN
#define ACTION_JOYSTICK_R_LEFT      MOUSE_MOVE_LEFT
#define ACTION_JOYSTICK_R_RIGHT     MOUSE_MOVE_RIGHT
#define ACTION_JOYSTICK_R_PRESS     MOUSE_LEFT
#define ACTION_JOYSTICK_R_MAX       BUTTON_UNDEFINED

// Right Button Actions
#define ACTION_BTN_R1               MOUSE_LEFT
#define ACTION_BTN_R2               KEY_SPACE   // Top button
#define ACTION_BTN_R3               KEY_R       // Middle button
#define ACTION_BTN_R4               KEY_E       // Bottom button

#endif // CONFIG_H
