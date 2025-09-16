# Gamepad Button Assignments

This document describes the button-to-action mappings for the LatteDeck gamepad controller.

## Overview

The gamepad assignments are defined in `gamepad_assignment.h` and can be easily customized by modifying the action definitions in that file.

## Special Actions

- `ACTION_NONE`: Used for buttons/actions that should have no effect
- This allows you to disable specific buttons without removing their pin definitions

## Button Layout

### Left Side (Movement Controls)
- **Left Joystick**: WASD movement controls
- **L1-L4 Buttons**: Various game actions

### Right Side (Mouse Controls)  
- **Right Joystick**: Mouse movement
- **R1-R4 Buttons**: Mouse clicks and additional actions

## Current Assignments

| Component        | Action                    | Key/Function        | Description                    |
|------------------|---------------------------|---------------------|--------------------------------|
| **Left Joystick** |                           |                     |                                |
|                  | Up                        | w                   | Move forward                   |
|                  | Down                      | s                   | Move backward                  |
|                  | Left                      | a                   | Move left                      |
|                  | Right                     | d                   | Move right                     |
|                  | Press                     | Space               | Jump                           |
|                  | Max (Sprint)              | e                   | Sprint when pushed to edge     |
| **Left Buttons** |                           |                     |                                |
|                  | L1 (Shoulder)             | Right Mouse Click   | Secondary action               |
|                  | L2 (Top)                  | Q                   | Quick action                   |
|                  | L3 (Middle)               | 1                   | Hotkey 1                       |
|                  | L4 (Bottom)               | 2                   | Hotkey 2                       |
| **Right Joystick** |                          |                     |                                |
|                  | Up                        | Mouse Up            | Move mouse up                  |
|                  | Down                      | Mouse Down          | Move mouse down                |
|                  | Left                      | Mouse Left          | Move mouse left                |
|                  | Right                     | Mouse Right         | Move mouse right               |
|                  | Press                     | Left Mouse Click    | Primary mouse click            |
|                  | Max                       | None                | No action                      |
| **Right Buttons** |                          |                     |                                |
|                  | R1 (Shoulder)             | Left Mouse Click    | Primary mouse click            |
|                  | R2 (Top)                  | Space               | Jump                           |
|                  | R3 (Middle)               | R                   | Reload/Interact                |
|                  | R4 (Bottom)               | E                   | Use/Interact                   |

## Customization

To modify button assignments:

1. Open `gamepad_assignment.h`
2. Change the `#define` values for the actions you want to modify
3. Use `ACTION_NONE` to disable a button
4. Recompile and upload to your device

### Example Customizations

```cpp
// Disable L4 button
#define ACTION_BTN_L4                  ACTION_NONE

// Change L2 to F key
#define ACTION_BTN_L2                  'f'

// Change R3 to Tab key
#define ACTION_BTN_R3                  '\t'

// Disable sprint functionality
#define ACTION_JOYSTICK_L_MAX          ACTION_NONE

// Disable a directional key
#define ACTION_JOYSTICK_L_UP           ACTION_NONE
```

**Note**: When using `ACTION_NONE`, the button or joystick action will be completely ignored - no input will be sent to the system.

## Available Key Constants

The system supports standard keyboard keys and mouse actions:

- **Keyboard Keys**: Character literals like `'a'`, `'b'`, `' '` (space), `'\t'` (tab), etc.
- **Mouse Actions**: `MOUSE_LEFT`, `MOUSE_RIGHT`, `MOUSE_MIDDLE`
- **Mouse Movement**: `MOUSE_MOVE_UP`, `MOUSE_MOVE_DOWN`, `MOUSE_MOVE_LEFT`, `MOUSE_MOVE_RIGHT`
- **Special**: `ACTION_NONE` (no action)

## Hardware Notes

- Pin assignments are defined in `gamepad_pinout.h`
- Button states are read with pull-up resistors
- Joystick deadzones are configurable in `gamepad_pinout.h`
