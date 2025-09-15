#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>
#include "composite_hid.h"

#define BUTTON_UNDEFINED 0

// --------------------------------------------------------
// Configuration constants
// --------------------------------------------------------

#define JOYSTICK_SIDE_MAX       500
#define SPRINT_THRESHOLD        480
#define JOYSTICK_X_DEADZONE     20
#define JOYSTICK_Y_DEADZONE     20


// --------------------------------------------------------
// Action assignments
// --------------------------------------------------------
#define ACTION_JOYSTICK_L_UP        'w'
#define ACTION_JOYSTICK_L_DOWN      's'
#define ACTION_JOYSTICK_L_LEFT      'a'
#define ACTION_JOYSTICK_L_RIGHT     'd'
#define ACTION_JOYSTICK_L_PRESS     ' '  // Space key
#define ACTION_JOYSTICK_L_MAX       'e'  // Sprint when moving joystick beyond threshold
#define ACTION_BTN_L1               MOUSE_RIGHT  
#define ACTION_BTN_L2               'q'  // Top button
#define ACTION_BTN_L3               '1'  // Middle button
#define ACTION_BTN_L4               '2'  // Bottom button

#define ACTION_JOYSTICK_R_UP        MOUSE_MOVE_UP
#define ACTION_JOYSTICK_R_DOWN      MOUSE_MOVE_DOWN
#define ACTION_JOYSTICK_R_LEFT      MOUSE_MOVE_LEFT
#define ACTION_JOYSTICK_R_RIGHT     MOUSE_MOVE_RIGHT
#define ACTION_JOYSTICK_R_PRESS     MOUSE_LEFT
#define ACTION_JOYSTICK_R_MAX       BUTTON_UNDEFINED
#define ACTION_BTN_R1               MOUSE_LEFT
#define ACTION_BTN_R2               ' '  // Top button
#define ACTION_BTN_R3               'r'  // Middle button
#define ACTION_BTN_R4               'e'  // Bottom button

// --------------------------------------------------------
// Pin definitions
// --------------------------------------------------------
#define PIN_GAMEPAD_ENABLE  4

// Left side
#define PIN_JOYSTICK_L_X     A5
#define PIN_JOYSTICK_L_Y     A4
#define PIN_JOYSTICK_L_SEL   21 

#define PIN_BTN_L1           7      // Shoulder button
#define PIN_BTN_L2           8      // Top button
#define PIN_BTN_L3           11     // Middle button
#define PIN_BTN_L4           12     // Bottom button

// Right side
#define PIN_JOYSTICK_R_X     A2
#define PIN_JOYSTICK_R_Y     A1
#define PIN_JOYSTICK_R_SEL   18

#define PIN_BTN_R1           1      // Shoulder button
#define PIN_BTN_R2           1//4      // Top button
#define PIN_BTN_R3           5      // Middle button
#define PIN_BTN_R4           6      // Bottom button


// --------------------------------------------------------
// Function prototypes
// --------------------------------------------------------

void setupGamepad();
void loopGamepad();

#endif // GAMEPAD_H
