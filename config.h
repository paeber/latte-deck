#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#include <HID-Project.h>
#include <HID-Settings.h>

// ============================================================================
// Hardware Configuration
// ============================================================================

// ============================================================================
// Gamepad Configuration
// ============================================================================
// Pin definitions are now in gamepad_pinout.h
#include "gamepad_pinout.h"

// ============================================================================
// Timing Configuration
// ============================================================================

#define MIN_UPDATE_INTERVAL         26      // Minimum update interval for USB-HID
#define MIN_SERIAL_REPORT_INTERVAL  5000    // Minimum interval for serial output

// ============================================================================
// Feature Enable Flags
// ============================================================================

// Debug configuration - uncomment to enable
//#define DEBUG_PRINT_GAMEPAD 1
#define DEBUG_PRINT_UPS 1

// Feature enable flags
#define ENABLE_MOUSE_KEYBOARD 1
#define ENABLE_HID_POWER_DEVICE 1

// ============================================================================
// Memory Configuration
// ============================================================================

#define DATA_LEN_MAX                0x24U
#define OUTPUT_BUFFER_SIZE          256     // Reduced from 512 to save memory
#define DEBUG_BUFFER_SIZE           80      // Reduced from 96 to save memory

// ============================================================================
// Action Assignments
// ============================================================================
// Button-to-action mappings are now in gamepad_assignment.h
#include "gamepad_assignment.h"

#endif // CONFIG_H
