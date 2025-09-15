#include <DFRobot_LPUPS.h>
#include "upsDef.h"

uint8_t regBuf[DATA_LEN_MAX] = { 0 };
DFRobot_LPUPS_I2C::sChargerStatus0_t chargerStatus0;
DFRobot_LPUPS_I2C::sChargerStatus1_t chargerStatus1;
DFRobot_LPUPS_I2C::sProchotStatus0_t prochotStatus0;
DFRobot_LPUPS_I2C::sProchotStatus1_t prochotStatus1;
uint16_t systemPower = 0, inputVoltage = 0;
uint16_t dischargeCurrent = 0, chargeCurrent = 0;
uint16_t CMPINVoltage = 0, inputCurrent = 0;
uint16_t batteryVoltage = 0, cellVoltage = 0, systemVoltage = 0, maxChargeVoltage = 0;
bool bCharging, bACPresent, bDischarging; // Whether charging, AC power present, discharging

char outputBuf[512]; // Print buffer

// String constants
const char STRING_DEVICE_CHEMISTRY[] PROGMEM = "Li-ion";   // Li-ion
const char STRING_OEM_VENDOR[] PROGMEM = "MyCoolUPS";
const char STRING_SERIAL[] PROGMEM = "UPS100";   // UPS100

//const byte bDeviceChemistry = IDEVICECHEMISTRY;   // Index of a string descriptor containing the battery’s chemistry.
//const byte bOEMVendor = IOEMVENDOR;

uint16_t iPresentStatus = 0;  // Now and previous device status.

byte bRechargable = 1;        // Rechargeable Battery (1)/Not Rechargeable Battery (0)
byte bCapacityMode = 2;       // In the data manual, "2" represents battery capacity in percentage.

// Physical parameters.
const uint16_t iConfigVoltage = MAX_BATTERY_VOLTAGE;   // Nominal value of the voltage.
uint16_t iVoltage = MAX_BATTERY_VOLTAGE;
uint16_t iRunTimeToEmpty = 0;
uint16_t iAvgTimeToFull = 3600 * 4; // 4 hours to charge
uint16_t iAvgTimeToEmpty = 7200;   // 12
uint16_t iRemainTimeLimit = 600;   // 1
/* Writing this value immediately shuts down (i.e., turns off) the output
   for a period equal to the indicated number of seconds in
   DelayBeforeReboot, after which time the output is started. */
int16_t  iDelayBe4Reboot = -1;
/* Writing this value shuts down (i.e., turns off) either the output after
  the indicated number of seconds, or sooner if the batteries become depleted. */
int16_t  iDelayBe4ShutDown = -1;

byte iAudibleAlarmCtrl = 2; // 1 - Disabled, 2 - Enabled, 3 - Muted

// Parameters compliant with Advanced Configuration and Power Interface (ACPI).
const byte iDesignCapacity = 100;
byte iWarnCapacityLimit = 10;         // warning at 10% 
byte iRemnCapacityLimit = 5;          // low at 5% 
const byte bCapacityGranularity1 = 1; // Battery capacity granularity between low and warning.
const byte bCapacityGranularity2 = 1; // Battery capacity granularity between warning and full.
byte iFullChargeCapacity = 100;


void initPowerDevice(void)
{
  // HID functionality is handled by CompositeHID::begin() in main setup()
  // This function is kept for compatibility but does nothing
}

void printChargeData(void)
{
  /*************** CS32_I2C_CHARGER_STATUS_REG ~ CS32_I2C_PROCHOT_STATUS_REG ***************/
  memcpy(&chargerStatus0, &regBuf[CS32_I2C_CHARGER_STATUS_REG], sizeof(regBuf[CS32_I2C_CHARGER_STATUS_REG]));
  memcpy(&chargerStatus1, &regBuf[CS32_I2C_CHARGER_STATUS_REG + 1], sizeof(regBuf[CS32_I2C_CHARGER_STATUS_REG + 1]));
  memcpy(&prochotStatus0, &regBuf[CS32_I2C_PROCHOT_STATUS_REG], sizeof(regBuf[CS32_I2C_PROCHOT_STATUS_REG]));
  memcpy(&prochotStatus1, &regBuf[CS32_I2C_PROCHOT_STATUS_REG + 1], sizeof(regBuf[CS32_I2C_PROCHOT_STATUS_REG + 1]));
  memset(outputBuf, 0, sizeof(outputBuf));
  sprintf(outputBuf, "Charge status register 0 = %#x\r\n"
    "Charge status register 1 = %#x\r\n"
    "Prochot status register 0 = %#x\r\n"
    "Prochot status register 1 = %#x\r\n",
    regBuf[CS32_I2C_CHARGER_STATUS_REG], regBuf[CS32_I2C_CHARGER_STATUS_REG + 1], regBuf[CS32_I2C_PROCHOT_STATUS_REG], regBuf[CS32_I2C_PROCHOT_STATUS_REG + 1]);
  Serial.print(outputBuf);

  /*************** CS32_I2C_ADC_PSYS_REG ~ CS32_I2C_ADC_VSYS_REG ***************/
  // PSYS: Full range: 3.06 V, LSB: 12 mV
  systemPower = regBuf[CS32_I2C_ADC_PSYS_REG] * 12;
  // VBUS: Full range: 3.2 V - 19.52 V, LSB: 64 mV
  inputVoltage = 3200 + regBuf[CS32_I2C_ADC_VBUS_REG] * 64;
  if (3200 == inputVoltage) {
    inputVoltage = 0;
  }
  // IDCHG: Full range: 32.512 A, LSB: 256 mA
  dischargeCurrent = regBuf[CS32_I2C_ADC_IDCHG_REG] * 256;
  // ICHG: Full range 8.128 A, LSB: 64 mA
  chargeCurrent = regBuf[CS32_I2C_ADC_ICHG_REG] * 64;
  // CMPIN: Full range 3.06 V, LSB: 12 mV
  CMPINVoltage = regBuf[CS32_I2C_ADC_CMPIN_REG] * 12;
  // POR State - IIN: Full range: 12.75 A, LSB: 50 mA
  inputCurrent = regBuf[CS32_I2C_ADC_IIN_REG] * 50;
  // VBAT: Full range : 2.88 V - 19.2 V, LSB 64 mV
  batteryVoltage = 2880 + regBuf[CS32_I2C_ADC_VBAT_REG] * 64;
  if (2880 == batteryVoltage) {
    batteryVoltage = 0;
  }
  cellVoltage = batteryVoltage / 3;
  // VSYS: Full range: 2.88 V - 19.2 V, LSB: 64 mV
  systemVoltage = 2880 + regBuf[CS32_I2C_ADC_VSYS_REG] * 64;
  if (2880 == systemVoltage) {
    systemVoltage = 0;
  }
  // VSYS: Full range: 2.88 V - 19.2 V, LSB: 64 mV
  systemVoltage = 2880 + regBuf[CS32_I2C_ADC_VSYS_REG] * 64;
  if (2880 == systemVoltage) {
    systemVoltage = 0;
  }
  maxChargeVoltage = LPUPS_CONCAT_BYTES(regBuf[CS32_I2C_SET_VBAT_LIMIT_REG + 1], regBuf[CS32_I2C_SET_VBAT_LIMIT_REG]);
  memset(outputBuf, 0, sizeof(outputBuf));
  sprintf(outputBuf, "System Power = %u mV\r\n"
    "Input Voltage = %u mV\r\n"
    "battery discharge current = %u mA\r\n"
    "battery charge current = %u mA\r\n"
    "CMPIN voltage = %u mV\r\n"
    "input current = %u mA\r\n"
    "battery voltage = %u mV\r\n"
    "cell voltage = %u mV\r\n"
    "system voltage = %u mV\r\n"
    "The max charge voltage = %u mV\r\n",
    systemPower, inputVoltage, dischargeCurrent, chargeCurrent, CMPINVoltage, inputCurrent, batteryVoltage, cellVoltage, systemVoltage, maxChargeVoltage);
  Serial.print(outputBuf);

}

void printChargeDataCompact(void)
{
  /*************** CS32_I2C_CHARGER_STATUS_REG ~ CS32_I2C_PROCHOT_STATUS_REG ***************/
  memcpy(&chargerStatus0, &regBuf[CS32_I2C_CHARGER_STATUS_REG], sizeof(regBuf[CS32_I2C_CHARGER_STATUS_REG]));
  memcpy(&chargerStatus1, &regBuf[CS32_I2C_CHARGER_STATUS_REG + 1], sizeof(regBuf[CS32_I2C_CHARGER_STATUS_REG + 1]));
  memcpy(&prochotStatus0, &regBuf[CS32_I2C_PROCHOT_STATUS_REG], sizeof(regBuf[CS32_I2C_PROCHOT_STATUS_REG]));
  memcpy(&prochotStatus1, &regBuf[CS32_I2C_PROCHOT_STATUS_REG + 1], sizeof(regBuf[CS32_I2C_PROCHOT_STATUS_REG + 1]));
  memset(outputBuf, 0, sizeof(outputBuf));

  /*************** CS32_I2C_ADC_PSYS_REG ~ CS32_I2C_ADC_VSYS_REG ***************/
  // PSYS: Full range: 3.06 V, LSB: 12 mV
  systemPower = regBuf[CS32_I2C_ADC_PSYS_REG] * 12;
  // VBUS: Full range: 3.2 V - 19.52 V, LSB: 64 mV
  inputVoltage = 3200 + regBuf[CS32_I2C_ADC_VBUS_REG] * 64;
  if (3200 == inputVoltage) {
    inputVoltage = 0;
  }
  // IDCHG: Full range: 32.512 A, LSB: 256 mA
  dischargeCurrent = regBuf[CS32_I2C_ADC_IDCHG_REG] * 256;
  // ICHG: Full range 8.128 A, LSB: 64 mA
  chargeCurrent = regBuf[CS32_I2C_ADC_ICHG_REG] * 64;
  // CMPIN: Full range 3.06 V, LSB: 12 mV
  CMPINVoltage = regBuf[CS32_I2C_ADC_CMPIN_REG] * 12;
  // POR State - IIN: Full range: 12.75 A, LSB: 50 mA
  inputCurrent = regBuf[CS32_I2C_ADC_IIN_REG] * 50;
  // VBAT: Full range : 2.88 V - 19.2 V, LSB 64 mV
  batteryVoltage = 2880 + regBuf[CS32_I2C_ADC_VBAT_REG] * 64;
  if (2880 == batteryVoltage) {
    batteryVoltage = 0;
  }
  cellVoltage = batteryVoltage / 3;
  // VSYS: Full range: 2.88 V - 19.2 V, LSB: 64 mV
  systemVoltage = 2880 + regBuf[CS32_I2C_ADC_VSYS_REG] * 64;
  if (2880 == systemVoltage) {
    systemVoltage = 0;
  }
  // VSYS: Full range: 2.88 V - 19.2 V, LSB: 64 mV
  systemVoltage = 2880 + regBuf[CS32_I2C_ADC_VSYS_REG] * 64;
  if (2880 == systemVoltage) {
    systemVoltage = 0;
  }
  maxChargeVoltage = LPUPS_CONCAT_BYTES(regBuf[CS32_I2C_SET_VBAT_LIMIT_REG + 1], regBuf[CS32_I2C_SET_VBAT_LIMIT_REG]);
  memset(outputBuf, 0, sizeof(outputBuf));
  sprintf(outputBuf, 
    "\nV_Sys = %5u mV | V_In  = %5u mV | I_In  = %5u mA\r\n"
    "V_bat = %5u mV | I_dis = %5u mA | I_chg = %5u mA\r\n"
    "V_cell = %4u mV | V_CMPIN = %5u mV\r\n"
    "SoC: %3d %%",
    systemVoltage, inputVoltage, inputCurrent, batteryVoltage, dischargeCurrent, chargeCurrent, cellVoltage, CMPINVoltage, iRemaining);
  Serial.print(outputBuf);
}

void printChargeDataCompactWithLimiter() {
    static unsigned long lastReportTime = 0;
    unsigned long currentMillis = millis();

    // Check if the interval has passed
    if (currentMillis - lastReportTime >= MIN_SERIAL_REPORT_INTERVAL) {
        lastReportTime = currentMillis;
        printChargeDataCompact(); // Call the existing compact charge report function
    }
}

void flashReportedData(void)
{
  // Charging status
  if (bCharging)
    bitSet(iPresentStatus, PRESENTSTATUS_CHARGING);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_CHARGING);

  // AC power supply
  if (bACPresent)
    bitSet(iPresentStatus, PRESENTSTATUS_ACPRESENT);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_ACPRESENT);

  // Fully charged
  if (iRemaining == iFullChargeCapacity)
    bitSet(iPresentStatus, PRESENTSTATUS_FULLCHARGE);
  else
    bitClear(iPresentStatus, PRESENTSTATUS_FULLCHARGE);

  // Discharging
  if (bDischarging) {   // Not charging
    bitSet(iPresentStatus, PRESENTSTATUS_DISCHARGING);
    // if(iRemaining < iRemnCapacityLimit) bitSet(iPresentStatus,PRESENTSTATUS_BELOWRCL);   // Below remaining capacity limit.

    // Exceeded runtime/capacity limit.
    if (iRunTimeToEmpty < iRemainTimeLimit)
      bitSet(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);
    else
      bitClear(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);

  } else {
    bitClear(iPresentStatus, PRESENTSTATUS_DISCHARGING);
    bitClear(iPresentStatus, PRESENTSTATUS_RTLEXPIRED);   // Clearing relevant flags during charging.
  }

  // Shutdown request.
  if (iDelayBe4ShutDown > 0) {
    bitSet(iPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);
    Serial.println("shutdown requested");
  } else
    bitClear(iPresentStatus, PRESENTSTATUS_SHUTDOWNREQ);

  // Shutdown imminent.
  if ((iPresentStatus & (1 << PRESENTSTATUS_SHUTDOWNREQ)) ||
    (iPresentStatus & (1 << PRESENTSTATUS_RTLEXPIRED))) {
    bitSet(iPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);   // - Shutdown imminent.
    Serial.println("shutdown imminent");
  } else
    bitClear(iPresentStatus, PRESENTSTATUS_SHUTDOWNIMNT);

  bitSet(iPresentStatus, PRESENTSTATUS_BATTPRESENT);   // - Power BATT
}

// Function to set the color of the RGB LED using PWM
void setRGBColor(uint8_t red, uint8_t green, uint8_t blue) {
    analogWrite(UPS_RED_LED, red);   // Set PWM for red channel
    analogWrite(UPS_GREEN_LED, green); // Set PWM for green channel
    analogWrite(UPS_BLUE_LED, blue);  // Set PWM for blue channel
}

// Function to initialize the RGB LED pins
void initRGBLed() {
    pinMode(UPS_RED_LED, OUTPUT);
    pinMode(UPS_GREEN_LED, OUTPUT);
    pinMode(UPS_BLUE_LED, OUTPUT);
}

// Function to set the RGB LED color with optional breathing effect
void setRGBColorWithEffect(uint8_t red, uint8_t green, uint8_t blue, bool pulse) {
    if (pulse) {
        static unsigned long lastUpdate = 0;
        static int brightness = 0;
        static int fadeAmount = 10; // Amount to change brightness each step

        unsigned long currentMillis = millis();

        // Update brightness every 30ms (adjust for desired speed)
        if (currentMillis - lastUpdate >= 30) {
            lastUpdate = currentMillis;

            // Set the LED color with the current brightness
            setRGBColor((red * brightness) / 255, (green * brightness) / 255, (blue * brightness) / 255);

            // Adjust brightness for breathing effect
            brightness += fadeAmount;
            if (brightness <= 0 || brightness >= 255) {
                fadeAmount = -fadeAmount; // Reverse direction
            }
        }
    } else {
        // Set the LED color directly without pulsing
        setRGBColor(red, green, blue);
    }
}
