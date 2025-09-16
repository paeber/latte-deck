#include "ups.h"


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