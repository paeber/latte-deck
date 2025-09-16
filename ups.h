#ifndef UPS_H
#define UPS_H

#include <Arduino.h>

// ============================================================================
// Hardware Configuration
// ============================================================================

// UPS Hardware Configuration
#ifndef UPS_I2C_ADDRESS
#define UPS_I2C_ADDRESS             0x55    // I2C address for UPS module
#endif

#ifndef THREE_BATTERIES_UPS_PID
#define THREE_BATTERIES_UPS_PID     0X42AA  // Product ID for 3-battery UPS
#endif

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

#endif // UPS_H