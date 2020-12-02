#ifndef SPI_H
#define SPI_H

#include "stm32f0xx.h"

void spi_init(void);

void spi_enable(void);
void spi_disable(void);

void spi_select_chip(void);
void spi_deselect_chip(void);

void spi_send_bulk(uint8_t * pData, uint16_t count);
void spi_recv_bulk(uint8_t * pData, uint16_t count);

void spi_send(uint8_t data);
uint8_t spi_recv(void);

#endif