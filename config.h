#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// Hardware Configuration
// ============================================================================

// UPS Hardware Configuration
#define UPS_I2C_ADDRESS             0x55    // I2C address for UPS module
#define THREE_BATTERIES_UPS_PID     0X42AA  // Product ID for 3-battery UPS

// LED Pin Definitions
#define UPS_GREEN_LED               9       // Battery level indicator LED, green
#define UPS_RED_LED                 10      // Battery level indicator LED, red
#define UPS_BLUE_LED                13      // Output refresh indicator, blue

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
// Battery Configuration
// ============================================================================

// Battery voltage range: 9.2V ~ 12.6V
#define MIN_VOLTAGE_MARGIN          200     // Add offset to minimal series voltage
#define N_CELLS_PACK                3       // 3 cells in series

#define MIN_CELL_VOLTAGE            2600    // From datasheet
#define NOM_CELL_VOLTAGE            3600    // From datasheet
#define MAX_CELL_VOLTAGE            4200    // From datasheet

#define CELL_CAPACITY_mAh           4000    // From datasheet
#define R_INTERNAL_mOHM             300     // Assumption

#define MIN_BATTERY_VOLTAGE         ((N_CELLS_PACK * MIN_CELL_VOLTAGE) + MIN_VOLTAGE_MARGIN)
#define MAX_BATTERY_VOLTAGE         (N_CELLS_PACK * MAX_CELL_VOLTAGE)

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
//#define DEBUG_PRINT_UPS 1
//#define DEBUG_PRINT_GAMEPAD 1

// Feature enable flags
#define ENABLE_MOUSE_KEYBOARD       1
#define ENABLE_HID_POWER_DEVICE     1

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
#define ACTION_JOYSTICK_L_UP        'w'
#define ACTION_JOYSTICK_L_DOWN      's'
#define ACTION_JOYSTICK_L_LEFT      'a'
#define ACTION_JOYSTICK_L_RIGHT     'd'
#define ACTION_JOYSTICK_L_PRESS     ' '     // Space key
#define ACTION_JOYSTICK_L_MAX       'e'     // Sprint when moving joystick beyond threshold

// Left Button Actions
#define ACTION_BTN_L1               MOUSE_RIGHT
#define ACTION_BTN_L2               'q'     // Top button
#define ACTION_BTN_L3               '1'     // Middle button
#define ACTION_BTN_L4               '2'     // Bottom button

// Right Joystick Actions (Mouse)
#define ACTION_JOYSTICK_R_UP        MOUSE_MOVE_UP
#define ACTION_JOYSTICK_R_DOWN      MOUSE_MOVE_DOWN
#define ACTION_JOYSTICK_R_LEFT      MOUSE_MOVE_LEFT
#define ACTION_JOYSTICK_R_RIGHT     MOUSE_MOVE_RIGHT
#define ACTION_JOYSTICK_R_PRESS     MOUSE_LEFT
#define ACTION_JOYSTICK_R_MAX       BUTTON_UNDEFINED

// Right Button Actions
#define ACTION_BTN_R1               MOUSE_LEFT
#define ACTION_BTN_R2               ' '     // Top button
#define ACTION_BTN_R3               'r'     // Middle button
#define ACTION_BTN_R4               'e'     // Bottom button

#endif // CONFIG_H
