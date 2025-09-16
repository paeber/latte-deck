// Include DFRobot LPUPS library for UPS communication only
// We'll handle HID reporting separately to avoid conflicts with NicoHood HID
#include <DFRobot_LPUPS.h>

#ifndef __UPS_UTILS_H__
#define __UPS_UTILS_H__

#include <Arduino.h>
#include "config.h"

// Forward declaration of LPUPS object
extern DFRobot_LPUPS_I2C LPUPS;

// Disable DFRobot library's HID functionality to avoid conflicts with NicoHood HID
// We'll implement custom HID reporting that works with both libraries
#ifndef UPS_HID_NICOHOOD
#define UPS_HID_NICOHOOD 0
#endif
extern uint8_t regBuf[DATA_LEN_MAX];

extern DFRobot_LPUPS_I2C::sChargerStatus1_t chargerStatus1;

extern uint16_t dischargeCurrent, chargeCurrent;
extern uint16_t batteryVoltage, maxChargeVoltage;
extern byte iRemaining;
extern bool bCharging, bACPresent, bDischarging; // Whether charging, AC power present, discharging

extern uint16_t iRunTimeToEmpty, iAvgTimeToEmpty;   // 12

extern uint16_t iPresentStatus;   // Now and previous device status.

// PresentStatus bit positions (Power Device spec – adjust if needed to match host expectations)
#define PRESENTSTATUS_CHARGING       0
#define PRESENTSTATUS_ACPRESENT      1
#define PRESENTSTATUS_FULLCHARGE     2
#define PRESENTSTATUS_DISCHARGING    3
#define PRESENTSTATUS_RTLEXPIRED     4
#define PRESENTSTATUS_SHUTDOWNREQ    5
#define PRESENTSTATUS_SHUTDOWNIMNT   6
#define PRESENTSTATUS_BATTPRESENT    7

// Initialize HID reporting using custom UPS HID implementation
void initPowerDevice(void);

// Use custom UPS HID implementation for battery reporting
// This works alongside NicoHood HID for mouse/keyboard functionality
void printChargeData(void);
void printChargeDataCompact(void);
void printChargeDataCompactWithLimiter(void);
void flashReportedData(void);

#endif /* __UPS_UTILS_H__ */
