/*!
 * @file  threeBatteriesLPUPS.ino
 * @brief LPUPS reports battery information to the computer via USB-HID
 * @details Reads battery information from UPS via I2C, and reports this information to the computer via USB-HID
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.1
 * @date  2023-08-09
 * @url  https://github.com/DFRobot/DFRobot_LPUPS
 */
#include <DFRobot_LPUPS.h>
#include "upsDef.h"
#include "ups_ctrl.h"

DFRobot_LPUPS_I2C LPUPS(&Wire, /*I2CAddr*/ UPS_I2C_ADDRESS);

uint16_t iPreviousStatus = 0;   // Now and previous device status.
byte iRemaining = 0, iPrevRemaining = 100;
int iRes = 0;
uint16_t iPrevRunTimeToEmpty = 0;

unsigned long iIntTimer = 0;            // Update interval counter
unsigned long lastRunUPS = 0; // Time when last run in milis()


// Moving Average
const int numReadings = 20;   // Number of readings for the moving average
int readings[numReadings];    // Array to store sensor readings
int readIndex = 0;            // Current index in the array
long total = 20*50;           // Sum of readings
int average = 50;             // Calculated moving average

int setupUPS(void)
{
  delay(5000);
  Serial.println("UPS init"); //Avoid serial port not working

  // Init the sensor
  int tryCount = 0;
  while (NO_ERR != LPUPS.begin(THREE_BATTERIES_UPS_PID)) {
    Serial.println("Communication with device failed, please check connection");
    tryCount++;
    if (tryCount > 3) {
      Serial.println("Max retries reached, giving up");
      return 1;
    }
    delay(2000);
  }

  // Initialize UPS indicator LEDs
  initRGBLed();

  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0; // Initialize all readings to zero
  }

  /**
   * @fn setMaxChargeVoltage
   * @brief Set the maximum charging voltage
   * @param data Maximum charging voltage:
   * @n          Three batteries: 11100 ~ 12600 mV
   * @n          Four batteries: 14800 ~ 16800 mV
   * @return None
   */ 
   // maxChargeVoltage = 11800;
   // LPUPS.setMaxChargeVoltage(maxChargeVoltage);

  // Initialize HID (stubbed when UPS_HID_NICOHOOD == 0)
  initPowerDevice();
  Serial.println("UPS init done");
  return 0;
}


// ----- Minimal Li-ion SoC from voltage (single cell) -----
// SoC (%) vs OCV (V) points (from ~0.8A curve; tweak if needed)
static const int N = 11;
static const uint16_t SOC_PCT[N]   = {  0,   10,  20,  30,  40,  50,  60,  70,  80,  90, 100};  // %
static const uint16_t OCV_mV_A8[N] = {2600,3000,3150,3300,3450,3600,3750,3850,3940,4040,4150};  // mV
static const uint16_t OCV_mV_2A[N] = {2600,2900,3070,3220,3370,3520,3670,3780,3900,3980,4100};  // mV

// Linear interpolation: voltage -> SoC
uint16_t socFromVoltage(uint16_t v_pack_mV, uint16_t dischargeCurrent_mA, uint16_t chargeCurrent_mA)
{
    uint16_t v_cell_mV = v_pack_mV / N_CELLS_PACK;

    // Compensate for internal resistance sag
    uint16_t v_rest_mV = v_cell_mV;
    #ifdef DEBUG_PRINT_UPS
    Serial.print(v_rest_mV);
    #endif
    
    // Discharge compensation
    if(dischargeCurrent_mA > 100){
      v_rest_mV = v_rest_mV + (dischargeCurrent_mA * R_INTERNAL_mOHM / 1000);
    }

    // Charge compensation
    if(chargeCurrent_mA > 100){
      //v_rest_mV = v_rest_mV - (chargeCurrent_mA * R_INTERNAL_mOHM / 1000);
    }

    #ifdef DEBUG_PRINT_UPS
    Serial.print("mV compensated to " );
    Serial.print(v_rest_mV);
    Serial.println("mV");
    #endif

    // Select proper OCV table
    const uint16_t *OCV_V = OCV_mV_A8;
    if (dischargeCurrent_mA > 1200) {
        OCV_V = OCV_mV_2A;
    }

    if (v_rest_mV <= OCV_V[0])     return SOC_PCT[0];
    if (v_rest_mV >= OCV_V[N - 1]) return SOC_PCT[N - 1];

    for (int i = 0; i < N - 1; ++i) {
        uint16_t v0 = OCV_V[i];
        uint16_t v1 = OCV_V[i + 1];
        if (v_rest_mV >= v0 && v_rest_mV <= v1) {
            float t = (float)(v_rest_mV - v0) / (float)(v1 - v0);
            return SOC_PCT[i] + (uint16_t)(t * (SOC_PCT[i + 1] - SOC_PCT[i]));
        }
    }

    return SOC_PCT[N - 1];
}


int addMovingAVG(int value){
  total -= readings[readIndex];               // Subtract the oldest reading
  readings[readIndex] = value;                // Read new value from sensor
  total += readings[readIndex];               // Add the new reading to the total
  readIndex = (readIndex + 1) % numReadings;  // Advance index, wrap around if needed
  average = total / numReadings;              // Calculate the average
  return average;
}


void loopUPS()
{
  unsigned long currentMillis = millis();
  iIntTimer = (currentMillis - lastRunUPS) / 1000;

  /************ Get charge chip data and print ****************************/
  /**
   * Get chip data
   * regBuf - data buffer for storing data
   */
  LPUPS.getChipData(regBuf);
  
  // Use the limited serial report function
  printChargeDataCompactWithLimiter();

  /*********** Unit of measurement, measurement unit ****************************/
  /**
   * Battery voltage range: 9.2V ~ 12.6V, in order to keep the battery stable at extreme values:
   * Assuming the battery voltage range is 9.3V ~ 12.5V, corresponding to battery capacity 0 ~ 100.
   * Note: You can adjust the battery capacity more accurately by correcting the voltage mutation with dischargeCurrent if interested.
   */
  if (batteryVoltage > MIN_BATTERY_VOLTAGE) {
    iRemaining = (((float)batteryVoltage - MIN_BATTERY_VOLTAGE) / (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE)) * 100;
  } else {
    Serial.println("The battery voltage is lower than normal !!!");   // Battery voltage lower than normal value.
  }

  iRemaining = socFromVoltage(batteryVoltage, dischargeCurrent, chargeCurrent);
  #ifdef DEBUG_PRINT_UPS
  Serial.print("SoC calc: ");
  Serial.print(iRemaining);
  #endif

  iRemaining = addMovingAVG(iRemaining);

  #ifdef DEBUG_PRINT_UPS
  Serial.print("% -> AVG: ");
  Serial.print(iRemaining);
  Serial.println("%");
  #endif

  if (100 < iRemaining) {
    iRemaining = 100;
  }

  // Please ensure to use the dedicated charger for LattePanda and connect it to the UPS (connect it to LP). 
  if (chargerStatus1.ac_stat) {   // check if there is charging current.
    bACPresent = true;
    if (64 < chargeCurrent) {   // Check if there is charging current. Due to precision issues, current less than 64 is considered as fully charged.
      bCharging = true;
    } else {
      bCharging = false;
    }
    bDischarging = false;
  } else {
    if (iPrevRemaining < iRemaining) {
      if (3 >= (iRemaining - iPrevRemaining))
        iRemaining = iPrevRemaining;
    }

    bACPresent = false;
    bCharging = false;
    if (dischargeCurrent) {   // Check if there is discharging current.
      bDischarging = true;
    } else {
      bDischarging = false;
    }
  }

  iRunTimeToEmpty = (float)iAvgTimeToEmpty * iRemaining / 100;

  // Adjust battery indicator LED based on the obtained battery capacity value iRemaining
  if (iRemaining <= 25) {
    setRGBColorWithEffect(255, 0, 0, bCharging);
  } else if ((iRemaining > 25) && (iRemaining < 75)) {
    setRGBColorWithEffect(255, 255, 0, bCharging);
  } else if (iRemaining >= 75) {
    setRGBColorWithEffect(0, 255, 0, bCharging);
  }

  // Refresh the values to be reported on USB-HID based on the obtained charge chip data
  flashReportedData();

  /************ Delay ***************************************/
  //delay(1000);
  //iIntTimer++;
  //digitalWrite(UPS_BLUE_LED, LOW);
  //delay(1000);
  //iIntTimer++;
  //digitalWrite(UPS_BLUE_LED, HIGH);

  /************ Report Battery to OS ***********************/
  if ((iPresentStatus != iPreviousStatus) || (iRemaining != iPrevRemaining) ||
    (iRunTimeToEmpty != iPrevRunTimeToEmpty) || (iIntTimer > MIN_UPDATE_INTERVAL)) {

#if UPS_HID_NICOHOOD
  // Send minimal CompositeHID Power reports
  CompositeHID::sendPowerRemaining(iRemaining);
  if (bDischarging) CompositeHID::sendPowerRuntime(iRunTimeToEmpty);
  iRes = CompositeHID::sendPowerStatus(iPresentStatus);
  pinMode(UPS_BLUE_LED, OUTPUT);
#else
    // HID reporting disabled; just toggle LED to indicate update
    iRes = 0;
    pinMode(UPS_BLUE_LED, OUTPUT);
#endif

    iIntTimer = 0;                          // Reset reporting interval timer
    lastRunUPS = currentMillis;             
    iPreviousStatus = iPresentStatus;       // Save new device status
    iPrevRemaining = iRemaining;            // Save new battery remaining capacity
    iPrevRunTimeToEmpty = iRunTimeToEmpty;  // Save new estimated battery runtime count

  }

  /************ Serial print reported battery level and operation result ******************/
  #ifdef DEBUG_PRINT_UPS
  Serial.print("iRunTimeToEmpty = "); // Estimated time to empty battery
  Serial.print(iRunTimeToEmpty);
  Serial.print(" | iRes = "); // Reporting return value, less than 0: indicates communication loss with host
  Serial.println(iRes);
  Serial.println();
  #endif
}
