#include <DFRobot_LPUPS.h>

#ifndef __LPUPS_DEF_H__
#define __LPUPS_DEF_H__

#include <Arduino.h>

// Disable DFRobot library's HID functionality to prevent conflicts
// We use our own CompositeHID system instead
#ifndef UPS_HID_NICOHOOD
#define UPS_HID_NICOHOOD 0
#endif

/**
 * Battery voltage range: 9.2V ~ 12.6V, in order to keep the battery stable at extreme values:
 * Assuming the battery voltage range is 9.3V ~ 12.5V, corresponding to battery capacity 0 ~ 100.
 * Note: You can adjust the battery capacity more accurately by correcting the voltage mutation with dischargeCurrent if interested.
 */

#define MIN_VOLTAGE_MARGIN  200     // Add offset to minimal series voltage
#define N_CELLS_PACK        3       // 3 cells in series

#define MIN_CELL_VOLTAGE    2600    // From datasheet
#define NOM_CELL_VOLTAGE    3600    // From datasheet
#define MAX_CELL_VOLTAGE    4200    // From datasheet

#define CELL_CAPACITY_mAh   4000    // From datasheet
#define R_INTERNAL_mOHM     300     // Assumption

#define MIN_BATTERY_VOLTAGE   ((N_CELLS_PACK * MIN_CELL_VOLTAGE) + MIN_VOLTAGE_MARGIN) //9300   // Lower battery voltage limit
#define MAX_BATTERY_VOLTAGE   (N_CELLS_PACK * MAX_CELL_VOLTAGE) //12500   // Upper battery voltage limit

#define UPS_GREEN_LED   9    // Battery level indicator LED, green
#define UPS_RED_LED     10   // Battery level indicator LED, red
#define UPS_BLUE_LED    13   // Output refresh every 1 second, indicates Arduino cycle is running, blue

#define MIN_UPDATE_INTERVAL         26      // Minimum update interval for USB-HID
#define MIN_SERIAL_REPORT_INTERVAL  5000    // Minimum interval for serial output

#define DATA_LEN_MAX   0x24U
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
#define HID_PD_REMAININGCAPACITY   CompositeHID::REPORT_ID_POWER_REMAINING
#define HID_PD_RUNTIMETOEMPTY      CompositeHID::REPORT_ID_POWER_RUNTIME
#define HID_PD_PRESENTSTATUS       CompositeHID::REPORT_ID_POWER_STATUS
void printChargeData(void);
void printChargeDataCompact(void);
void printChargeDataCompactWithLimiter(void);
void flashReportedData(void);

#endif /* __LPUPS_DEF_H__ */
