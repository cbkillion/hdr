#include "si446x.h"
#include "main.h"

/*const*/ uint8_t configuration_array[] = CONFIG_CMD_ARRAY;

void si446x_init(void)
{
	si446x_power_on_reset();
	si446x_configure(configuration_array);
	si446x_wait_for_cts();
	si446x_start_rx();

	// si446x_test_tx();
}

uint8_t si446x_command(uint8_t cmd, uint8_t * tx_buff, uint8_t tx_len, uint8_t * rx_buff, uint8_t rx_len)
{
	uint8_t status = 0;

	// send command
	spi_enable();
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
		spi_transfer(READ_CMD_BUFF);
		si446x_wait_for_cts();
		if (spi_transfer(0) == 0xFF)
		{
			spi_recv_bulk(rx_buff, rx_len);
		}
		spi_disable();
	}

	return status;
}

void si446x_power_on_reset(void)
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

void si446x_configure(/*const*/ uint8_t * commands)
{
	uint8_t cmd_length = commands[0];
	while (cmd_length > 0)
	{
		si446x_command(*(commands + 1), commands + 2, cmd_length - 1, 0, 0);
		commands += (cmd_length + 1);
		cmd_length = *commands;
	}
}

void si446x_send(uint8_t * buffer, uint16_t len)
{
	si446x_write_tx_fifo(buffer, len);
	si446x_start_tx();
	red_led(ON);
}

void si446x_write_tx_fifo(uint8_t * buffer, uint8_t len)
{
	spi_enable();
	spi_transfer(WRITE_TX_FIFO);
	spi_send_bulk(buffer, len);
	spi_disable();
}

void si446x_read_rx_fifo(void)
{
	// get the number of bytes in the rx fifo
	uint8_t num_bytes;
	si446x_command(FIFO_INFO, 0, 0, &num_bytes, 1);
	
	spi_enable();
	spi_transfer(READ_RX_FIFO);
	spi_recv_bulk(rx_buffer, num_bytes);
	spi_disable();
}

void si446x_start_tx(void)
{
	// enter RX state when done, use individual field settings
	uint8_t buffer[] = {/*channel*/ 0x00, 0x80, 0x00, 0x00, 0x00, 0x00};
	si446x_command(START_TX, buffer, sizeof(buffer), 0, 0);
}

void si446x_start_rx(void)
{
	uint8_t buffer[] = {/*channel*/ 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08};
	si446x_command(START_RX, buffer, sizeof(buffer), 0, 0);
}

void si446x_clear_rx_fifo(void)
{
	uint8_t fifo = 0x02;
	si446x_command(FIFO_INFO, &fifo, 1, 0, 0);
}

void si446x_clear_tx_fifo(void)
{
	uint8_t fifo = 0x01;
	si446x_command(FIFO_INFO, &fifo, 1, 0, 0);
}

void si446x_test_tx(void)
{
	si446x_power_on_reset();

	uint8_t buff[] = {0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80};
	si446x_command(POWER_UP, buff, sizeof(buff), 0, 0);

	uint8_t buff1[] = {0x00, 0x00, 0x00};
	si446x_command(GET_INT_STATUS, buff1, sizeof(buff1), 0, 0);

	uint8_t buff2[] = {0x00, 0x04, 0x00, 0x55, 0x00, 0x18, 0x06};
	si446x_command(SET_PROPERTY, buff2, sizeof(buff2), 0, 0); // GLOBAL CONFIG
	
	uint8_t buff3[] = {0x20, 0x01, 0x00, 0x15}; // 0x00 for CW, 0x1[1:5] for other mod types and PRN data
	si446x_command(SET_PROPERTY, buff3, sizeof(buff3), 0, 0); // MODEM CONTROL
	
	uint8_t buff4[] = {0x40, 0x06, 0x00, 0x3C, 0x08, 0x00, 0x00, 0x22, 0x22};
	si446x_command(SET_PROPERTY, buff4, sizeof(buff4), 0, 0); // RF FREQUENCY to 915M
	
	uint8_t buff5[] = {0x22, 0x03, 0x00, 0x20, 0x36, 0xC0};
	si446x_command(SET_PROPERTY, buff5, sizeof(buff5), 0, 0); // TX POWER

	uint8_t buff6[] = {0x00, 0x30, 0x1F, 0xFF};
	si446x_command(START_TX, buff6, sizeof(buff6), 0, 0); // START TX
	green_led(ON);
}