#ifndef SI446X_H
#define SI446X_H

#include "stm32f0xx.h"
#include "spi.h"

#define CTS_RETRIES             1000
#define CTS_SUCCESS             0xFF

// Commands
#define POWER_UP                0x02
#define NOP                     0x00
#define PART_INFO               0x01
#define FUNC_INFO               0x10
#define SET_PROPERTY            0x11
#define GET_PROPERTY            0x12
#define GPIO_PIN_CONFIG         0x13
#define FIFO_CONFIG             0x15
#define GET_INT_STATUS          0x20
#define REQUEST_DEVICE_STATE    0x33
#define CHANGE_STATE            0x34
#define READ_CMD_BUFF           0x44
#define FRR_A_READ              0x50
#define FRR_B_READ              0x51
#define FRR_C_READ              0x53
#define FRR_D_READ              0x57
#define IRCAL                   0x17
#define IRCAL_MANUAL            0x1A
#define START_TX                0x31
#define TX_HOP                  0x37
#define WRITE_TX_FIFO           0x66
#define PACKET_INFO             0x16
#define GET_MODEM_STATUS        0x22
#define START_RX                0x32
#define RX_HOP                  0x36
#define READ_RX_FIFO            0x77
#define GET_ADC_READING         0x14
#define GET_PH_STATUS           0x21
#define GET_CHIP_STATUS         0x23

void si446x_init(void);
void si446x_por(void);
uint8_t si446x_wait_for_cts(void);
uint8_t si446x_commmand(uint8_t cmd, uint8_t * tx_buff, uint8_t tx_len, uint8_t * rx_buff, uint8_t rx_len);

#endif