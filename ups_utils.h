#include <DFRobot_LPUPS.h>

#ifndef __UPS_UTILS_H__
#define __UPS_UTILS_H__

#include <Arduino.h>
#include "config.h"

// Disable DFRobot library's HID functionality to prevent conflicts
// We use our own CompositeHID system instead
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

// Initialize HID reporting (stubbed when UPS_HID_NICOHOOD == 0)
void initPowerDevice(void);

// Always use CompositeHID for HID functionality
#include "composite_hid.h"
// Map old names to CompositeHID helpers for minimal intrusion
#define HID_PD_REMAININGCAPACITY   LATTE_REPORT_ID_POWER_DEVICE
#define HID_PD_RUNTIMETOEMPTY      LATTE_REPORT_ID_POWER_DEVICE
#define HID_PD_PRESENTSTATUS       LATTE_REPORT_ID_POWER_DEVICE
void printChargeData(void);
void printChargeDataCompact(void);
void printChargeDataCompactWithLimiter(void);
void flashReportedData(void);

#endif /* __UPS_UTILS_H__ */
