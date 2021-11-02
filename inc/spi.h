#ifndef SPI_H
#define SPI_H

#include "stm32f0xx.h"

#define IO8(addr)   (*(volatile uint8_t *)(addr))
#define SPI1_DR     IO8(SPI1_BASE + 0x0C)

void    spi_init(void);

void    spi_enable(void);
void    spi_disable(void);
void    spi_send_bulk(uint8_t * pData, uint8_t count);
void    spi_recv_bulk(uint8_t * pData, uint8_t count);
uint8_t spi_transfer(uint8_t data);

#endif