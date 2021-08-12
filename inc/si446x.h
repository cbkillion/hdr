#ifndef SI446X_H
#define SI446X_H

#include "stm32f0xx.h"
#include "spi.h"
#include "si446x_config.h"

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
#define FIFO_INFO               0x15
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

#define PACKET_SENT_INT         0x20
#define PACKET_RECV_INT         0x10
#define CRC_ERROR_INT           0x08
#define TX_FIFO_EMPTY_INT       0x02
#define RX_FIFO_FULL_INT        0x01
#define INVALID_SYNC_INT        0x20
#define PACKET_HANDLER_INT      0x01
#define MODEM_INT               0x02
#define CHIP_INT                0x04

void    si446x_init(void);
void    si446x_power_on_reset(void);
uint8_t si446x_wait_for_cts(void);
uint8_t si446x_command(uint8_t cmd, uint8_t * tx_buff, uint8_t tx_len, uint8_t * rx_buff, uint8_t rx_len);
void    si446x_pin_config(void);
void    si446x_configure(/*const*/ uint8_t * commands);

void    si446x_send(uint8_t * buffer, uint16_t len);
void    si446x_write_tx_fifo(uint8_t * buffer, uint8_t len);
void    si446x_clear_tx_fifo(void);
void    si446x_read_rx_fifo(void);
void    si446x_clear_rx_fifo(void);

uint8_t si446x_set_properties(uint16_t first_property, uint8_t count, uint8_t * properties);
uint8_t si446x_get_properties(uint16_t first_property, uint8_t count, uint8_t * properties);

void    si446x_start_tx(void);
void    si446x_start_rx(void);

void    si446x_test_tx(void);

uint8_t si446x_addr;

#endif