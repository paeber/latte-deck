#ifndef HID_CONFIG_H
#define HID_CONFIG_H

// HID Configuration for Composite Device with Report IDs
// This file configures Report IDs for all HID interfaces to work with Windows

// Include HID-Project library configuration
#include "config.h"

// Override HID-Project settings to enable Report IDs for composite devices
// This is necessary when combining multiple HID interfaces (Mouse + Keyboard)

// Enable Report IDs for Power Device interface
#ifndef HID_POWER_DEVICE_REPORT_ID
#define HID_POWER_DEVICE_REPORT_ID 1
#endif

// UPS Power Device specific configuration
#ifndef UPS_HID_POWER_DEVICE_REPORT_ID
#define UPS_HID_POWER_DEVICE_REPORT_ID 1
#endif

// Enable Report IDs for Mouse interface
#ifndef HID_MOUSE_REPORT_ID
#define HID_MOUSE_REPORT_ID 2
#endif

// Enable Report IDs for Keyboard interface  
#ifndef HID_KEYBOARD_REPORT_ID
#define HID_KEYBOARD_REPORT_ID 3
#endif

// Enable Report IDs for Consumer Control interface (if used)
#ifndef HID_CONSUMER_REPORT_ID
#define HID_CONSUMER_REPORT_ID 4
#endif

// Enable Report IDs for System Control interface (if used)
#ifndef HID_SYSTEM_REPORT_ID
#define HID_SYSTEM_REPORT_ID 5
#endif

// Enable Report IDs for Gamepad interface (if used)
#ifndef HID_GAMEPAD_REPORT_ID
#define HID_GAMEPAD_REPORT_ID 6
#endif

// Force HID-Project to use Report IDs
#ifndef HID_USE_REPORT_IDS
#define HID_USE_REPORT_IDS 1
#endif

// Enable composite device support
#ifndef HID_COMPOSITE_DEVICE
#define HID_COMPOSITE_DEVICE 1
#endif

#endif // HID_CONFIG_H
