#include "main.h"

int main(void)
{
	configure_clock();
	gpio_init();
	spi_init();
	si446x_init();
 
	// usb_init();
	uint8_t tx_test[] = {0, 0, 0, 'A', 'D', '0', 'Z', 'F', 4, 'p', 'i', 'n', 'g'};
	uint8_t pushed = 0;
	uint8_t interrupt_status[9];
	uint8_t rx_buffer[64];
	uint8_t rx_len;

    while (1)
	{
		if (!gpio_read_pin(nIRQ_PORT, nIRQ_PIN))
		{
			si446x_command(GET_INT_STATUS, 0, 0, interrupt_status, sizeof(interrupt_status));
			if (interrupt_status[3] & 0x10) // packet received
			{
				green_led_on();
				si446x_clear_rx_fifo();
				// si446x_read_rx_fifo(rx_buffer, &rx_len);
			}

			if (interrupt_status[3] == 0x08) // crc error...
			{
				red_led_on();
			}
		}


		if (read_button() && !pushed)
		{
			red_led_off();
			green_led_off();

			pushed = 1;
			si446x_send(tx_test, sizeof(tx_test), 0);
			
			// wait for packet sent interrupt, then clear by reading
			while (gpio_read_pin(nIRQ_PORT, nIRQ_PIN));
			si446x_command(GET_INT_STATUS, 0, 0, interrupt_status, sizeof(interrupt_status));
		} 
		if (!read_button())
		{
			pushed = 0;
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
	RCC->CFGR |= RCC_CFGR_PLLMUL10; // set PLLMUL to 6 (HSI / 2 * 10 = 40 MHz)
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

void red_led_on(void)
{
	gpio_write_pin(RED_LED_PORT, RED_LED_PIN, 1);
}

void red_led_off(void)
{
	gpio_write_pin(RED_LED_PORT, RED_LED_PIN, 0);
}

void green_led_on(void)
{
	gpio_write_pin(GREEN_LED_PORT, GREEN_LED_PIN, 1);
}

void green_led_off(void)
{
	gpio_write_pin(GREEN_LED_PORT, GREEN_LED_PIN, 0);
}

/*  PINOUT
	---------------
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