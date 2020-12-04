#ifndef SPI_H
#define SPI_H

#include "stm32f0xx.h"

#define IO8(addr)       (*(volatile uint8_t *)(addr))

void spi_init(void);

void spi_enable(void);
void spi_disable(void);

void spi_send_bulk(uint8_t * pData, uint8_t count);
void spi_recv_bulk(uint8_t * pData, uint8_t count);

void spi_send(uint8_t data);
uint8_t spi_recv(void);
uint8_t spi_transfer(uint8_t data);

#endif