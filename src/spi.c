#include "main.h"
#include "spi.h"
#include "gpio.h"

void spi_init(void)
{
	// initialize the SPI1 bus
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 clock
	SPI1->CR1 |= (SPI_CR1_MSTR | (1 << 3)); // Master mode; rate = Fpclk / 2
	SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);
	SPI1->CR1 |= SPI_CR1_SPE; // Does this need to be here?
}

void spi_enable(void)
{
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_disable(void)
{
	while (SPI1->SR & SPI_SR_FTLVL); // wait until FTLVL = 0
	while (SPI1->SR & SPI_SR_BSY); // wait until BSY = 0
	SPI1->CR1 &= ~SPI_CR1_SPE; // disable SPI --> releases NSS
	while (SPI1->SR & SPI_SR_FRLVL) // read everything out of the buffer
		IO8(&SPI1->DR);
	SPI1->SR; // resets the overrun flag
}

void spi_send_bulk(uint8_t * pData, uint8_t count)
{
	uint8_t _count = count;

	while (_count--)
		spi_transfer(*pData++);
}

void spi_recv_bulk(uint8_t * pData, uint8_t count)
{
	uint8_t _count = count;

	while (_count--)
		*pData++ = spi_transfer(0);
}

uint8_t spi_transfer(uint8_t data)
{
	while(!(SPI1->SR & SPI_SR_TXE)); // wait for tx buffer to be empty
	IO8(&SPI1->DR) = data;	// start the transfer
	while(!(SPI1->SR & SPI_SR_RXNE)); // wait for the data to send
	return IO8(&SPI1->DR); // read the response to clear the rx buffer
}