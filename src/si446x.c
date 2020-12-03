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

	uint8_t buff2[3];
	if (si446x_commmand(REQUEST_DEVICE_STATE, 0, 0, buff2, sizeof(buff2)))
		red_led_on();

	if((buff2[1] == 0xFF))
		green_led_on();
}

uint8_t si446x_commmand(uint8_t cmd, uint8_t * tx_buff, uint8_t tx_len, uint8_t * rx_buff, uint8_t rx_len)
{
	uint8_t status = 0;
	// uint16_t count = CTS_RETRIES;

	// send command
	spi_select_chip();
	spi_send(cmd);
	if (tx_len && tx_buff)
		spi_send_bulk(tx_buff, tx_len);
	spi_deselect_chip();

	si446x_wait_for_cts();

	// check CTS and read response if required
	// while(count-- && status)
	// {	
	// 	spi_select_chip();
	// 	spi_send(READ_CMD_BUFF); // check CTS
	// 	if (spi_recv() == CTS_SUCCESS)
	// 	{
			if(rx_len && rx_buff)
			{
				spi_select_chip();
				spi_send(READ_CMD_BUFF);
				spi_recv_bulk(rx_buff, rx_len);
				spi_deselect_chip();
			}
			// status = 0;
	// 	}
	// 	// delay(1);
	// 	spi_deselect_chip();
	// }

	return status;
}

void si446x_por(void)
{
	gpio_write_pin(SDN_PORT, SDN_PIN, 1);
	delay(10);
	gpio_write_pin(SDN_PORT, SDN_PIN, 0);

	si446x_wait_for_cts();
}

void si446x_wait_for_cts(void)
{
	while(!gpio_read_pin(CTS_PORT, CTS_PIN));
}