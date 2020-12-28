#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

#include "si446x_patch.h"

/*
	Crystal Freq: 30000000     
	Mod Type: 4GFSK
	Symbol Rate: 100000
	Freq deviation: 25000     
	RF Freq.(MHz): 902   

	RX IF frequency is  -468750 Hz
	WB filter 2 (BW = 274.83 kHz)
	NB-filter 2 (BW = 274.83 kHz)

	Modulation index: 0.5
*/

// CONFIGURATION COMMANDS
// Power up with patch applied, EZRadio mode, 30 MHz crystal
#define RF_POWER_UP 0x02, 0x81, 0x00, 0x01, 0xC9, 0xC3, 0x80

// GPIO0: POR, GPIO1: CTS, GPIO2: RX_FIFO_FULL, GPIO3: TX_FIFO_EMPTY, low drive strength
#define RF_GPIO_PIN_CFG 0x13, 0x00, 0x00, 0x22, 0x23, 0x00, 0x00, 0x60

// Set value of XO tuning capacitors in Global Clock Config
#define RF_GLOBAL_XO_TUNE_2 0x11, 0x00, 0x01, 0x00, 0x55

// Enable packet handler and modem interrupts
// Enable specific interrupts: packet sent, packet received, crc error, invalid sync
#define RF_INT_CTL_ENABLE_2 0x11, 0x01, 0x03, 0x00, 0x03, 0x38, 0x20

// Disable fast-response registers
#define RF_FRR_CTL_A_MODE_4 0x11, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00

// Standard preamble, 40 symbols in length, 16 bit threshold
#define RF_PREAMBLE_TX_LENGTH_9 0x11, 0x10, 0x05, 0x00, 0x05, 0x10, 0x00, 0x0F, 0x31

// Sync word is 0x2DD4 as 4FSK mod type, one error allowed but only at the beginning of the sync word
#define RF_SYNC_CONFIG_6 0x11, 0x11, 0x06, 0x00, 0x19, 0xB4, 0x2B, 0x00, 0x00, 0x80

/*
	Select IEC-16 CRC polynomial and seed with all zeros
	Select a PN11 whitening polynomial and seed with all ones
	Enable 4FSK modulation and send CRC MSB first
	Setup variable length payload: length value is 1 byte, MSB first, length is put in FIFO, field 2 is length, field 3 is variable
	Packet length adjust = 0
*/
#define RF_PKT_CRC_CONFIG_12 0x11, 0x12, 0x0B, 0x00, 0x02, 0x05, 0x00, 0xFF, 0xFF, 0x20, 0x22, 0x00, 0x2B, 0x02, 0x00

/*
	Set TX FIFO threshold to 0x30
	Set RX FIFO threshold to 0x30
	Setup field 1: 8 bytes long, 4FSK, restart the whitening PRNG, restart CRC, transmit CRC, check CRC in rx, enable CRC
	Setup field 2: 1 byte long, 4FSK, transmit CRC, check CRC in rx, enable CRC
*/
#define RF_PKT_RX_THRESHOLD_12 0x11, 0x12, 0x0A, 0x0B, 0x30, 0x30, 0x00, 0x08, 0x14, /*0xAA*/ 0x00, 0x00, 0x01, 0x10, /*0x2A*/ 0x00

// Setup field 3: up to 255 bytes long, 4FSK, whitening disabled, transmit CRC, check CRC in rx, enable CRC
#define RF_PKT_FIELD_3_CRC_CONFIG_12 0x11, 0x12, 0x04, 0x15, 0x00, 0xFF, 0x10, /*0x2A*/ 0x00

// Setup 4GFSK modulation using FIFO as source, 20x oversampling, 200Kbps data rate
#define RF_MODEM_MOD_TYPE_12 0x11, 0x20, 0x0A, 0x00, 0x05, 0x00, 0x07, 0x1E, 0x84, 0x80, 0x09, 0xC9, 0xC3, 0x80

// Setup 25KHz frequency deviation
#define RF_MODEM_FREQ_DEV_0_1 0x11, 0x20, 0x03, 0x0A, 0x00, 0x0A, 0x3D

// Set PA rampdown delay, fixed IF freq, rx decimation, bit clock recovery settings
#define RF_MODEM_TX_RAMP_DELAY_12 0x11, 0x20, 0x0C, 0x18, 0x05, 0x00, 0x08, 0x03, 0xC0, 0x00, 0x00, 0x10, 0x00, 0xE8, 0x00, 0x64

// Configure bit clock recovery NCO, gain and other settings, automatic frequency correction settings
#define RF_MODEM_BCR_NCO_OFFSET_2_12 0x11, 0x20, 0x0C, 0x24, 0x05, 0x1E, 0xB8, 0x03, 0x6A, 0x02, 0x02, 0x00, 0x00, 0x23, 0x83, 0x6A

// More automatic frequency correction settings, AGC settings
#define RF_MODEM_AFC_LIMITER_1_3 0x11, 0x20, 0x06, 0x30, 0x01, 0x19, 0xA0, 0x00, 0x00, 0xE0

// More AGC settings, FSK slicer settings and symbol mapping
#define RF_MODEM_AGC_WINDOW_SIZE_12 0x11, 0x20, 0x0C, 0x38, 0x11, 0x16, 0x16, 0x00, 0x1A, 0x20, 0x00, 0x00, 0x28, 0x0C, 0xA4, 0x23

// RX filter coefficients
#define RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12 0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C
#define RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12 0x11, 0x21, 0x0C, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5
#define RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12 0x11, 0x21, 0x0C, 0x18, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00

// Setup power amplifier mode to medium power (4461), class E/square wave match, moderate power (2F), clock duty, PA ramp speed and FSK ramp delay
#define RF_PA_MODE_4 0x11, 0x22, 0x04, 0x00, 0x20, 0x36, 0xC0, 0x1D

// Various PLL settings
#define RF_SYNTH_PFDCP_CPFF_7 0x11, 0x23, 0x07, 0x00, 0x34, 0x04, 0x0B, 0x04, 0x07, 0x70, 0x03

// Setup RF frequency: 902 MHz base frequency with 250 KHz channel step sizes
#define RF_FREQ_CONTROL_INTE_8 0x11, 0x40, 0x06, 0x00, 0x3B, 0x09, 0x11, 0x11, 0x22, 0x22

#define CONFIG_CMD_ARRAY { \
        SI446X_PATCH_CMDS, \
        0x07, RF_POWER_UP, \
        0x08, RF_GPIO_PIN_CFG, \
        0x05, RF_GLOBAL_XO_TUNE_2, \
        0x07, RF_INT_CTL_ENABLE_2, \
        0x08, RF_FRR_CTL_A_MODE_4, \
        0x09, RF_PREAMBLE_TX_LENGTH_9, \
        0x0A, RF_SYNC_CONFIG_6, \
        0x0F, RF_PKT_CRC_CONFIG_12, \
        0x0E, RF_PKT_RX_THRESHOLD_12, \
        0x08, RF_PKT_FIELD_3_CRC_CONFIG_12, \
        0x0E, RF_MODEM_MOD_TYPE_12, \
        0x07, RF_MODEM_FREQ_DEV_0_1, \
        0x10, RF_MODEM_TX_RAMP_DELAY_12, \
        0x10, RF_MODEM_BCR_NCO_OFFSET_2_12, \
        0x0A, RF_MODEM_AFC_LIMITER_1_3, \
        0x10, RF_MODEM_AGC_WINDOW_SIZE_12, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12, \
        0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12, \
        0x08, RF_PA_MODE_4, \
        0x0B, RF_SYNTH_PFDCP_CPFF_7, \
        0x0A, RF_FREQ_CONTROL_INTE_8, \
        0x00 \
}

#endif