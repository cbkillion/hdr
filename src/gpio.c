#include "gpio.h"

void gpio_init(void)
{
	// stetup pins on ports A, B, and F
	RCC->AHBENR 	= 0x00460014;
	GPIOA->MODER	= 0x0000A940; // 3, 4: output; 5, 6, 7: AF; 11, 12: ...? (USB)
	GPIOA->OSPEEDR  = 0x0000FF00; // SPI pins to high speed
	GPIOB->MODER	= 0x00005000; // 6, 7: output;
	GPIOF->MODER	= 0x00000000; // all inputs;
}

void gpio_write_pin(GPIO_TypeDef * port, uint16_t pin, uint8_t value)
{
	if(value)
		port->BSRR = (uint32_t) (1 << pin);
	else
		port->BRR = (uint32_t) (1 << pin);
}

uint16_t gpio_read_pin(GPIO_TypeDef * port, uint16_t pin)
{
	return port->IDR & (1 << pin);
}
