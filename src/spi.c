#include "main.h"
#include "spi.h"
#include "gpio.h"

void spi_init(void)
{
	// initialize the SPI1 bus at 10 MHz
	// with hardware NSS control (requires pullup on NSS pin)
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 clock
	SPI1->CR1 |= (SPI_CR1_MSTR);// | (1 << 4); // Master mode; rate = Fpclk / 2
	SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);
	SPI1->CR1 |= SPI_CR1_SPE;

	// DMA...?
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
	uint8_t * _pData = pData;

	while (_count--)
		spi_transfer(*_pData++);

}

void spi_recv_bulk(uint8_t * pData, uint8_t count)
{
	// wait for SPI to be done with what it's doing
	while (SPI1->SR & SPI_SR_BSY);

	// clear out the rx buffer
	while (SPI1->SR & SPI_SR_RXNE)
		IO8(&SPI1->DR);

	uint8_t * _pData = pData;
	uint8_t _count = count;

	while (_count--)
		*_pData++ = spi_transfer(0);

}

uint8_t spi_transfer(uint8_t data)
{
	// send out one byte of data and
	// read one byte from the rx buffer

	while(!(SPI1->SR & SPI_SR_TXE)); // wait for tx buffer to be empty
	IO8(&SPI1->DR) = data;	// start the transfer
	while(SPI1->SR & SPI_SR_BSY); // wait for the data to send (should we wait on RXNE...?)
	return IO8(&SPI1->DR); // read the respons to clear the rx buffer
}