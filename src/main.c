#include "main.h"

int main(void)
{
	configure_clock();
	gpio_init();
	spi_init();
	si446x_init();
	interrupts_init();

	flash_led(0x22);

	// usb_init();

	uint8_t syn[] = {0, 0, 0, 0, 5, 'A', 'D', '0', 'Z', 'F'};
	uint8_t pushed = 0;
	new_rx_data = 0;

	// TODO: move back to 2-byte length and max of 1024 bytes in packet
	// TODO: handle FIFO full and empty events so we can tx/rx packets larger than 64 bytes
	// TODO: USB comms so we can get some data in and out
	// TODO: make a class or struct defining message rather than raw array

	while (1)
	{
		if (new_rx_data)
		{
			new_rx_data = 0;
			if (rx_buffer[5] == 'A')
				green_led(ON);
		}

		// si446x_send(syn, sizeof(syn));
		// delay(5000000);
		if (read_button() && !pushed)
		{
			green_led(OFF);
			pushed = 1;
		}

		if (!read_button() && pushed)
		{
			pushed = 0;
			si446x_send(syn, sizeof(syn));
		}
	}
}

void configure_clock(void)
{
	// enable flash prefetch buffer and waitstates
	FLASH->ACR |= (FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY);

	// disable the PLL and wait for it to turn off
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY) {}

	// modify the PLL settings
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2; // set PLL source to be HSI / 2 (4 MHz)
	RCC->CFGR |= RCC_CFGR_PLLMUL10; // set PLLMUL to 10 (HSI / 2 * 10 = 40 MHz)
	RCC->CFGR |= RCC_CFGR_PPRE_DIV2; // set PClk to SysClock / 2 (20 MHz)

	// enable the PLL and wait for it to turn on
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}

	// switch the SysClock over to the PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// wait for the SysClock to switch over to the PLL
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL) {}
}

uint16_t read_button(void)
{
	return gpio_read_pin(BUTTON_PORT, BUTT0N_PIN);
}

void delay(uint32_t delay)
{
	for (uint32_t ii = 0; ii < delay; ii++)
		asm("NOP");
}

void red_led(uint8_t state)
{
	gpio_write_pin(RED_LED_PORT, RED_LED_PIN, state);
}

void green_led(uint8_t state)
{
	gpio_write_pin(GREEN_LED_PORT, GREEN_LED_PIN, state);
}

void flash_led(uint8_t num_flashes)
{
	uint8_t red_flashes = (num_flashes >> 4) & 0x0F;
	uint8_t green_flashes = num_flashes & 0x0F;

	for (int ii = 0; ii < red_flashes; ii++)
	{
		red_led(ON);
		delay(20000);
		red_led(OFF);
		delay(500000);
	}
	for (int ii = 0; ii < green_flashes; ii++)
	{
		green_led(ON);
		delay(20000);
		green_led(OFF);
		delay(500000);
	}
}

void nss_low(void)
{
	gpio_write_pin(NSS_PORT, NSS_PIN, OFF);
}

void nss_high(void)
{
	gpio_write_pin(NSS_PORT, NSS_PIN, ON);
}


/*  PINOUT
	---------------
	GPIO0 	--> PA8 -- 	INPUT
	GPIO1 	--> PB1 -- 	INPUT
	GPIO2 	--> PA2 -- 	INPUT
	GPIO3 	--> PA1 -- 	INPUT
	nIRQ  	--> PB0 -- 	INPUT
	---------------
	SDN 	--> PA3 -- 	OUTPUT
	---------------
	nSEL 	--> PA4 --  OUTPUT
	SCLK 	--> PA5 --  SPI
	MISO 	--> PA6 --  SPI
	MOSI 	--> PA7 --  SPI
	---------------
	D- 		--> PA11 --  USB
	D+ 		--> PA12 --  USB
	---------------
	LED_R	-->	PB6 --  OUTPUT
	LED_G	--> PB7 --  OUTPUT
	---------------
	SWITCH	--> PB8 --  INPUT
*/