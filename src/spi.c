#include "main.h"
#include "spi.h"
#include "gpio.h"

void spi_init(void)
{
	// initialize the SPI1 bus at 10 MHz with software NSS control
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 clock
	SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);// | (1 << 4); // Master mode; rate = Fpclk / 2
	SPI1->CR2 |= SPI_CR2_FRXTH;
	SPI1->CR1 |= SPI_CR1_SPE;

	spi_deselect_chip();

	// DMA...?
}

void spi_enable(void)
{
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_disable(void)
{
	volatile uint8_t tmp;

	while (SPI1->SR & SPI_SR_FTLVL); // wait until FTLVL = 0
	while (SPI1->SR & SPI_SR_BSY); // wait until BSY = 0
	SPI1->CR1 &= ~SPI_CR1_SPE; // disable SPI; release NSS
	while (SPI1->SR & SPI_SR_FRLVL)
		tmp = *(uint8_t*)&SPI1->DR;
	tmp = SPI1->SR;
}

void spi_send_bulk(uint8_t * pData, uint8_t count)
{
	uint8_t _count = count;
	uint8_t * _pData = pData;

	while (_count--)
		spi_send(*_pData++);
}

void spi_send(uint8_t data)
{
	while (!(SPI1->SR & SPI_SR_TXE));
	IO8(&SPI1->DR) = data;
	// *(uint8_t *)&SPI1->DR = data;
}

void spi_recv_bulk(uint8_t * pData, uint8_t count)
{
	uint8_t * _pData = pData;
	uint8_t _count = count;

	while (_count--)
		*_pData++ = spi_recv();
}

uint8_t spi_recv(void)
{	
	// must send data to read data
	while (!(SPI1->SR & SPI_SR_TXE));
	IO8(&SPI1->DR) = 0;
	// *(uint8_t*)&SPI1->DR = (uint8_t) 0;
	
	while (!(SPI1->SR & SPI_SR_RXNE));
	return IO8(&SPI1->DR);
	// return (*(uint8_t*)&SPI1->DR);
}

void spi_select_chip(void)
{
	gpio_write_pin(NSS_PORT, NSS_PIN, 0);
}

void spi_deselect_chip(void)
{
	gpio_write_pin(NSS_PORT, NSS_PIN, 1);
}