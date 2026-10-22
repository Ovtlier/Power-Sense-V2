/*
 * array_power_sense.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#ifndef INC_DEVICE_ARRAY_POWER_SENSE_H_
#define INC_DEVICE_ARRAY_POWER_SENSE_H_

/* Functionality */
#define SOFT_START_VCAN_OUT
#define PRECHARGE_HV
#define ADC_CS_TIED_LOW 1  // CS is tied low on Array board side

/* External ADC scaling
 * AINCOM is tied to VREF_2V5 (2.5V) on the Array board, so single-ended
 * measurements read (V_AINx - 2.5V).  The conversion formula must add back
 * the 2.5V offset before applying the divider scale:
 *   V_actual = (signed_count * COUNTS_TO_V + AINCOM_OFFSET) * VSCALE
 */
#define EXT_ADC_VREF            2.5f
#define EXT_ADC_FULLSCALE       8388608.0f   // 2^23
#define EXT_ADC_COUNTS_TO_V     (EXT_ADC_VREF / EXT_ADC_FULLSCALE)
#define EXT_ADC_AINCOM_OFFSET   2.5f         // AINCOM = REFOUT = 2.5V on Array board

#define VSCALE_V3V3             4.0f         // 3V3_ISO → R2(30K) → AIN0 → R9(10K) → GND
#define VSCALE_V5V              4.0f         // 5V_ISO  → R3(30K) → AIN1 → R10(10K) → GND
#define VSCALE_V_ARRAY          37.585f      // 3x1M series / 82K = 3082K/82K (confirmed from schematic)
#define VSCALE_V_MPPT           37.585f      // same divider as ARRAY_OUT (confirmed from schematic)
// I_ARRAY: differential (AIN5-AIN2), 2x INA190A2, gain=50, shunt=0.5mΩ
// Reference cancels in differential → I(A) = V_diff / 0.025

/* NTC thermistor: 0603R-NTC-10K, B25/50 = 3380K
 * BOARD_TEMP0 (under relay) = ADC1_IN14, BOARD_TEMP1 (near ADC) = ADC3_IN12
 * Circuit: 3V3 → 10K pullup → ADC → NTC → GND */
#define NTC_R25             10000.0f     // resistance at 25°C (Ω)
#define NTC_BETA            3380.0f      // B25/50 (K)
#define NTC_PULLUP          10000.0f     // pullup resistor (Ω)
#define NTC_V_SUPPLY        3.3f         // pullup rail voltage (V)

/* Precharge */
#define PRECHARGE_TIMEOUT_MS    10000
#define PRECHARGE_RATIO         0.7f         // close relay when V_array >= 0.7 * V_mppt

/* Fan control (CANOUT_EN = power to fans) */
#define FAN_ON_TEMP_C           46.0f
#define FAN_OFF_TEMP_C          44.0f

/* CAN IDs */
#define CAN_ID_ARRAY_COMMAND    0x5F9
#define CAN_CMD_HV_ON           0x01
#define CAN_CMD_HV_OFF          0x00
#define CAN_ID_MPPT_TEMP_BASE   0x600        // temp messages at 0x602, 0x612, 0x622, 0x632, 0x642
#define CAN_ID_TX_OUT_BUS       0x5F1        // float ARRAY_OUT (V), float I_ARRAY (A)
#define CAN_ID_TX_IN_BUS        0x5F3        // float MPPT_IN (V)
#define CAN_ID_TX_SUPPLY        0x5F4        // uint16 VCAN(mV), ICANOUT(mA), VCANOUT(mV), IBOARD(mA)
#define CAN_ID_TX_STATUS        0x5F5        // status bitfields, precharge timer, 5V(mV), 3V3(mV)
#define CAN_ID_TX_TEMP          0x5F7        // uint16 board temp, output term temp, input term temp
#define N_MPPTS                 5

/* CANOUT supply thresholds */
#ifdef SOFT_START_VCAN_OUT
#define VCAN_OUT_THRESHOLD_MIN  11000
#define VCAN_OUT_THRESHOLD_MAX  15500
#endif

#endif /* INC_DEVICE_ARRAY_POWER_SENSE_H_ */
