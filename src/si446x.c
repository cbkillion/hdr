#include "si446x.h"
#include "main.h"

void si446x_init(void)
{
	si446x_por();

	uint8_t buff[] = {0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80};
	if (si446x_commmand(POWER_UP, buff, sizeof(buff), 0, 0))
		red_led_on();

	uint8_t buff1[] = {0x00, 0x00, 0x00};
	if (si446x_commmand(GET_INT_STATUS, buff1, sizeof(buff1), 0, 0))
		red_led_on();

	uint8_t buff2[9];
	if (si446x_commmand(PART_INFO, 0, 0, buff2, sizeof(buff2)))
		red_led_on();

	if((buff2[3] == 0x61))
		green_led_on();
}

uint8_t si446x_commmand(uint8_t cmd, uint8_t * tx_buff, uint8_t tx_len, uint8_t * rx_buff, uint8_t rx_len)
{
	uint8_t status = 0;

	// send command
	spi_enable();
	// spi_send(cmd);
	spi_transfer(cmd);
	if (tx_len && tx_buff)
		spi_send_bulk(tx_buff, tx_len);
	spi_disable();

	// wait for radio to process
	si446x_wait_for_cts();

	// read response if required
	if(rx_len && rx_buff)
	{
		spi_enable();
		// spi_send(READ_CMD_BUFF);
		spi_transfer(READ_CMD_BUFF);
		spi_recv_bulk(rx_buff, rx_len);
		spi_disable();
	}

	return status;
}

void si446x_por(void)
{
	gpio_write_pin(SDN_PORT, SDN_PIN, 1);
	delay(15);
	gpio_write_pin(SDN_PORT, SDN_PIN, 0);

	si446x_wait_for_cts();
}

uint8_t si446x_wait_for_cts(void)
{
	// TODO: add a timeout
	while(!gpio_read_pin(CTS_PORT, CTS_PIN));
	
	// if timeout, return something other than 0
	return 0;
}
