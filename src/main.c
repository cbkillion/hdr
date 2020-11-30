#include "main.h"

int main(void)
{
	configure_clock();
	init_gpio();
	// init_spi();
	// init_usb();
	// init_radio();
 
    while (1)
	{
		if (check_button())
		{
			GPIOB->ODR = GPIO_ODR_6 | GPIO_ODR_7;
		}
		else
		{
			GPIOB->ODR = 0x00000000;
		}
		
	}
}

void init_gpio(void)
{
	// stetup pins on ports A, B, and F
	RCC->AHBENR 	= 0x00460014;
	GPIOA->MODER	= 0x0000AA40; // 3: output; 4, 5, 6, 7: AF; 11, 12: ...? (USB)
	GPIOB->MODER	= 0x00005000; // 6, 7: output;
	GPIOF->MODER	= 0x00000000; // all inputs;
}

void write_pin(GPIO_TypeDef * port, uint16_t pin, uint8_t value)
{
	if(value)
		port->BSRR = (uint32_t) pin;
	else
		port->BRR = (uint32_t) pin;
}

uint16_t read_pin(GPIO_TypeDef * port, uint16_t pin)
{
	return port->IDR & pin;
}

uint16_t check_button(void)
{
	return read_pin(BUTTON_PORT, BUTT0N_PIN);
}

void configure_clock(void)
{
	// configure SysClock to run at 48 MHz from PLL and HSI oscillator
	
	// enable flash prefetch buffer and waitstates
	FLASH->ACR = 0x00000011;

	// disable the PLL and wait for it to turn off
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY) {}

	// modify the PLL settings
	RCC->CFGR = 0x00100000; // Set PLLMUL to 12 (HSI / 2 * 6 = 48 MHz)

	// enable the PLL and wait for it to turn on
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}

	// switch the SysClock over to the PLL
	RCC->CFGR |= 0x00000002;

	// wait for the SysClock to switch over to the PLL
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL) {}
}






/*
PINOUT:
	GPIO0 	--> PB2 -- 	INPUT
	GPIO1 	--> PB1 -- 	INPUT
	GPIO2 	--> PA2 -- 	INPUT
	GPIO3 	--> PA1 -- 	INPUT
	nIRQ  	--> PB0 -- 	INPUT
	---------------
	SDN 	--> PA3 -- 	OUTPUT
	---------------
	nSEL 	--> PA4 --  SPI
	SCLK 	--> PA5 --  SPI
	MISO 	--> PA6 --  SPI
	MOSI 	--> PA7 --  SPI
	---------------
	D+ 		--> PA11 --  USB
	D- 		--> PA12 --  USB
	---------------
	LED_R	-->	PB6 --  OUTPUT
	LED_G	--> PB7 --  OUTPUT
	---------------
	SWITCH	--> PF11 --  INPUT
*/