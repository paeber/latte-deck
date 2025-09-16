
// ============================================================================
// Gamepad Configuration
// ============================================================================

#define JOYSTICK_SIDE_MAX           500
#define SPRINT_THRESHOLD            480
#define JOYSTICK_X_DEADZONE         20
#define JOYSTICK_Y_DEADZONE         20

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