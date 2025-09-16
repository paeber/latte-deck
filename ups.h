#ifndef UPS_H
#define UPS_H

#include <Arduino.h>

// ============================================================================
// Hardware Configuration
// ============================================================================

// UPS Hardware Configuration
// Note: UPS_I2C_ADDRESS and THREE_BATTERIES_UPS_PID are defined in DFRobot_LPUPS.h

#define UPS_STATUS_LED              13      // Charging steady, blinking based on remaining capacity

// ============================================================================
// Battery Configuration
// ============================================================================

// Battery voltage range: 9.2V ~ 12.6V
#define MIN_VOLTAGE_MARGIN          200     // Add offset to minimal series voltage
#define N_CELLS_PACK                3       // 3 cells in series

#define MIN_CELL_VOLTAGE            2600    // From datasheet
#define NOM_CELL_VOLTAGE            3600    // From datasheet
#define MAX_CELL_VOLTAGE            4200    // From datasheet

#define CELL_CAPACITY_mAh           4000    // From datasheet
#define R_INTERNAL_mOHM             300     // Assumption

#define MIN_BATTERY_VOLTAGE         ((N_CELLS_PACK * MIN_CELL_VOLTAGE) + MIN_VOLTAGE_MARGIN)
#define MAX_BATTERY_VOLTAGE         (N_CELLS_PACK * MAX_CELL_VOLTAGE)

// ============================================================================
// Function Prototypes
// ============================================================================

// Battery capacity calculation
uint16_t socFromVoltage(uint16_t v_pack_mV, uint16_t dischargeCurrent_mA, uint16_t chargeCurrent_mA);

#endif // UPS_H