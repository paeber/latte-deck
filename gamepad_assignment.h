#ifndef GAMEPAD_ASSIGNMENT_H
#define GAMEPAD_ASSIGNMENT_H

#include <Arduino.h>
#include "config.h"


// ============================================================================
// Gamepad Button Assignment Configuration
// ============================================================================
// This file defines which buttons and joystick actions map to which keys/actions
// Modify these definitions to customize your gamepad behavior

// ============================================================================
// Gamepad Configuration Constants
// ============================================================================

// Joystick Inversion Settings
#define JOYSTICK_R_INVERT_X         (1)     // Non-inverted right joystick X-axis
#define JOYSTICK_R_INVERT_Y         (-1)    // Invert right joystick Y-axis
#define JOYSTICK_L_INVERT_X         (-1)    // Invert left joystick X-axis
#define JOYSTICK_L_INVERT_Y         (1)     // Invert left joystick Y-axis

// Sensitivity and Threshold Settings
#define JOYSTICK_MOUSE_SENSITIVITY  350     // Mouse sensitivity (higher = slower)
#define JOYSTICK_SIDE_MAX           500     // Maximum joystick value for clipping
#define SPRINT_THRESHOLD            480     // Threshold for sprint activation
#define SPRINT_THRESHOLD_ENABLED    0       // Enable sprint functionality for left joystick
#define JOYSTICK_X_DEADZONE         10      // Deadzone for X-axis to prevent drift
#define JOYSTICK_Y_DEADZONE         10      // Deadzone for Y-axis to prevent drift
#define JOYSTICK_BINARY_THRESHOLD   200     // Threshold for binary joystick movement

// ============================================================================
// Special Action Definitions
// ============================================================================

// Define for buttons/actions that should have no effect
#define ACTION_NONE                    0

// ============================================================================
// Left Joystick Actions
// ============================================================================
// Left joystick is typically used for movement (WASD keys)

#define ACTION_JOYSTICK_L_UP           'w'
#define ACTION_JOYSTICK_L_DOWN         's'
#define ACTION_JOYSTICK_L_LEFT         'a'
#define ACTION_JOYSTICK_L_RIGHT        'd'
#define ACTION_JOYSTICK_L_PRESS        ' '         // Space character
#define ACTION_JOYSTICK_L_MAX          'e'         // Sprint when moving joystick beyond threshold

// ============================================================================
// Left Button Actions
// ============================================================================
// Left side buttons (L1-L4)

#define ACTION_BTN_L1                  MOUSE_RIGHT
#define ACTION_BTN_L2                  'q'       // Top button
#define ACTION_BTN_L3                  'r'       // Middle button
#define ACTION_BTN_L4                  'n'       // Bottom button

// ============================================================================
// Right Joystick Actions
// ============================================================================
// Right joystick is typically used for mouse movement

#define ACTION_JOYSTICK_R_UP           MOUSE_MOVE_UP
#define ACTION_JOYSTICK_R_DOWN         MOUSE_MOVE_DOWN
#define ACTION_JOYSTICK_R_LEFT         MOUSE_MOVE_LEFT
#define ACTION_JOYSTICK_R_RIGHT        MOUSE_MOVE_RIGHT
#define ACTION_JOYSTICK_R_PRESS        MOUSE_LEFT
#define ACTION_JOYSTICK_R_MAX          ACTION_NONE // No action for right joystick max

// ============================================================================
// Right Button Actions
// ============================================================================
// Right side buttons (R1-R4)

#define ACTION_BTN_R1                  MOUSE_LEFT
#define ACTION_BTN_R2                  ' '      // Top button
#define ACTION_BTN_R3                  'e'      // Middle button
#define ACTION_BTN_R4                  '2'      // Bottom button


#endif // GAMEPAD_ASSIGNMENT_H
