#ifndef GAMEPAD_ASSIGNMENT_H
#define GAMEPAD_ASSIGNMENT_H

#include <Arduino.h>
#include <HID-Project.h>


// ============================================================================
// Gamepad Button Assignment Configuration
// ============================================================================
// This file defines which buttons and joystick actions map to which keys/actions
// Modify these definitions to customize your gamepad behavior

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
#define ACTION_BTN_L2                  KEY_Q       // Top button
#define ACTION_BTN_L3                  KEY_1       // Middle button
#define ACTION_BTN_L4                  KEY_2       // Bottom button

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
#define ACTION_BTN_R2                  KEY_SPACE   // Top button
#define ACTION_BTN_R3                  KEY_R       // Middle button
#define ACTION_BTN_R4                  KEY_E       // Bottom button

// ============================================================================
// Assignment Summary Table
// ============================================================================
/*
| Component        | Action                    | Key/Function        |
|------------------|---------------------------|---------------------|
| Left Joystick    | Up                        | W                   |
|                  | Down                      | S                   |
|                  | Left                      | A                   |
|                  | Right                     | D                   |
|                  | Press                     | Space               |
|                  | Max (Sprint)              | E                   |
| Left Buttons     | L1 (Shoulder)             | Right Mouse Click   |
|                  | L2 (Top)                  | Q                   |
|                  | L3 (Middle)               | 1                   |
|                  | L4 (Bottom)               | 2                   |
| Right Joystick   | Up                        | Mouse Up            |
|                  | Down                      | Mouse Down          |
|                  | Left                      | Mouse Left          |
|                  | Right                     | Mouse Right         |
|                  | Press                     | Left Mouse Click    |
|                  | Max                       | None                |
| Right Buttons    | R1 (Shoulder)             | Left Mouse Click    |
|                  | R2 (Top)                  | Space               |
|                  | R3 (Middle)               | R                   |
|                  | R4 (Bottom)               | E                   |
*/

#endif // GAMEPAD_ASSIGNMENT_H
