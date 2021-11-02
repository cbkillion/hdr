#include "cbk_usb.h"
#include "main.h"

USBInfoTypeDef usb;

uint8_t device_desc[DEVICE_DESC_LENGTH] =
{
	DEVICE_DESC_LENGTH, 	/*bLength */
	0x01,				 	/*bDescriptorType*/
	0x00, 					/*bcdUSB */
	0x02,
	0x00, 					/*bDeviceClass*/
	0x00, 					/*bDeviceSubClass*/
	0x00, 					/*bDeviceProtocol*/
	EP0_MAX_SIZE, 			/*bMaxPacketSize*/
	LOBYTE(VID), 			/*idVendor*/
	HIBYTE(VID), 			/*idVendor*/
	LOBYTE(PID), 			/*idProduct*/
	HIBYTE(PID), 			/*idProduct*/
	0x00, 					/*bcdDevice rel. 1.00*/
	0x01,
	0x01, 					/*Index of manufacturer  string*/
	0x02, 					/*Index of product string*/
	0x00, 					/*Index of serial number string*/
	0x01 					/*bNumConfigurations*/
};

uint8_t device_qual_desc[DQ_DESC_LENGTH] =
{
	DQ_DESC_LENGTH,
	0x06,
	0x00,
	0x02,
	0x00,
	0x00,
	0x00,
	EP0_MAX_SIZE,
	0x01,
	0x00
};

uint8_t config_desc[TOTAL_CONFIG_DESC_LENGTH] =
{
	// Configuration descriptor
	CONFIG_DESC_LENGTH, // Config desc length
	0x02,				// Descriptor type
	TOTAL_CONFIG_DESC_LENGTH, 0x00,	// Total length
	0x01,				// Num interfaces
	0x01,				// Configuration value
	0x00,				// String index
	0x80,				// Attributes
	0x32,				// Max current

	// Interface descriptor
	INTF_DESC_LENGTH,	// Intf desc length
	0x04,				// Descriptor type
	0x00,				// Intf number
	0x00,				// Alternate setting
	0x01,				// Num endpoints
	0x03,				// Intf class
	0x00,				// Intf subclass
	0x00,				// Intf protocol
	0x00,				// Intf string index

	// HID descriptor
	0x09,				// HID desc length
	0x21,				// Desc type
	0x11, 0x01,			// HID spec
	0x00,				// Country code
	0x01,				// Num descriptors to follow
	0x22,				// Desc type to follow
	HID_REPORT_DESC_LENGTH, 0x00,// Desc size to follow

	// Endpoint descriptor
	0x07,	 			// Endpoint desc length
	0x05,				// Desc type
	0x81,				// Endpoint num and dir (1 and IN)
	0x03,				// Transfer type (Interrupt)
	HID_REPORT_SIZE, 0x00, // Max packet size
	0x05				// Polling interval (milliseconds)
};

uint8_t HID_Desc[HID_DESC_LENGTH] =
{
	0x09,				// HID desc length
	0x21,				// Desc type
	0x11, 0x01,			// HID spec
	0x00,				// Country code
	0x01,				// Num descriptors to follow
	0x22,				// Desc type to follow
	HID_REPORT_DESC_LENGTH, 0x00, // Desc size to follow
};

// Custom HID desc
uint8_t HID_Report_Desc[HID_REPORT_DESC_LENGTH] =
{
	0x05, 0x01,			// usage page - generic desktop
	0x09, 0x00,			// usage - undefined
	0xA1, 0x01,			// collection (application)

	0x15, 0x00,			// logical minimum - 0
	0x26, 0xFF, 0x00,	// logical maximum - 255

	0x85, 0x01,			// report id - 1
	0x75, 0x08,			// report size - 8 bits
	0x95, HID_REPORT_SIZE, // report count - 12 bytes
	0x09, 0x00,			// usage - undefined
	0x81, 0x82,			// input - data, var, abs, vol

	0xC0				// end collection
};


uint8_t lang_id[4] =
{
	0x04, 0x03, 0x09, 0x04
};

uint8_t manf_string[MANF_STR_LENGTH] =
{
	MANF_STR_LENGTH,
	0x03,
	'K',0,
	'i',0,
	'l',0,
	'l',0,
	'i',0,
	'o',0,
	'n',0,
	' ',0,
	'E',0,
	'n',0,
	'g',0,
	'i',0,
	'n',0,
	'e',0,
	'e',0,
	'r',0,
	'i',0,
	'n',0,
	'g',0
};

uint8_t product_string[PROD_STR_LENGTH] =
{
	PROD_STR_LENGTH,
	0x03,
	'U',0,
	'S',0,
	'B',0,
	' ',0,
	'S',0,
	'c',0,
	'a',0,
	'l',0,
	'a',0,
	'r',0,
	' ',0,
	'N',0,
	'e',0,
	't',0,
	'w',0,
	'o',0,
	'r',0,
	'k',0,
	' ',0,
	'A',0,
	'n',0,
	'a',0,
	'l',0,
	'y',0,
	'z',0,
	'e',0,
	'r',0
};

void init_usb(void)
{
	// Enable the USB peripheral clocks and interrupts
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
	// Use PLL Clock for USB peripheral
	RCC->CFGR3 |= RCC_CFGR3_USBSW;

	NVIC_SetPriority(USB_IRQn, 0);
	NVIC_EnableIRQ(USB_IRQn);

	// Force reset of USB peripheral
	USB->CNTR = USB_CNTR_FRES;
	USB->CNTR = 0x0000;

	// Clear all USB interrupts
	USB->ISTR = 0x0000;

	// Set the USB buffer table address
	USB->BTABLE = 0x0000;

	// Set the interrupt mask
	USB->CNTR = USB_CNTR_CTRM | USB_CNTR_ERRM | USB_CNTR_WKUPM /*| USB_CNTR_SUSPM*/ | USB_CNTR_RESETM;

	// Start the USB device
	USB->BCDR |= USB_BCDR_DPPU;
}

void reset_usb(void)
{
	open_ep(0x80, USB_EP_CONTROL, EP0_MAX_SIZE, 0x0040); // control IN  ep (device to host)
	open_ep(0x00, USB_EP_CONTROL, EP0_MAX_SIZE, 0x0080); // control OUT ep (host to device)

	usb.ep0_state = IDLE;
	USB->DADDR = USB_DADDR_EF;
	usb.device_state = DEFAULT_STATE;
}

void handle_ctr(void)
{
	uint16_t * ep_reg;
	pma_table * ep_tbl;
	uint16_t istr_val, addr;
	uint8_t ep;
	uint16_t count = 0;

	while (((istr_val = USB->ISTR) & USB_ISTR_CTR))
	{
		ep = (uint8_t) (istr_val & USB_ISTR_EP_ID);
		ep_reg = EPR(ep);
		ep_tbl = EPT(ep);

		if (ep)
		{
			if (*ep_reg & USB_EP_CTR_RX)
			{
				// out transaction/endpoint
				EP_CLEAR_CTR_RX(ep_reg);
				count = ep_tbl->rx.count & 0x03FF;

				if (count)
				{
					uint8_t out_data[count];
					read_pm(count, ep_tbl->rx.addr, out_data);
				}

				EP_RX_VALID(ep_reg);
			}

			if (*ep_reg & USB_EP_CTR_TX)
			{
				// in transaction/endpoint
				EP_CLEAR_CTR_TX(ep_reg);
				usb.int_in_ep_state = IDLE;
			}
		}
		else
		{
			if (*ep_reg & USB_EP_CTR_TX)
			{ // IN transaction complete
				EP_CLEAR_CTR_TX(ep_reg);
				in_transaction_callback(ep);
				EP_RX_VALID(ep_reg);

				if (usb.device_addr_to_set)
				{
					USB->DADDR = (usb.device_addr_to_set | USB_DADDR_EF);
					usb.device_addr_to_set = 0;
				}
			}
			else
			{ // OUT transaction complete
				if (*ep_reg & USB_EP_SETUP)
				{ // setup transaction
					EP_CLEAR_CTR_RX(ep_reg);

					count = ep_tbl->rx.count & 0x03FF;
					addr = ep_tbl->rx.addr;

					uint8_t setup_packet[8];
					read_pm(count, addr, setup_packet);
					process_setup_packet(setup_packet);

					EP_RX_VALID(ep_reg);
				}
				else if (*ep_reg & USB_EP_CTR_RX)
				{ // control data OUT transaction
					EP_CLEAR_CTR_RX(ep_reg);

					// reading and processing control out data would happen here

					EP_RX_VALID(ep_reg);
				}
			}
		}
	}
}

void in_transaction_callback(uint8_t ep_num)
{
	if (usb.ep0_state == DATA_IN)
		usb.ep0_state = STATUS_OUT;
	else
		usb.ep0_state = IDLE;
}

void open_ep(uint8_t ep, uint16_t ep_type, uint16_t ep_max_size, uint16_t pm_addr)
{
	uint16_t * ep_reg = EPR(ep);
	pma_table * ep_tbl = EPT(ep);

	*ep_reg = ep_type | (ep & 0x07);

	if (ep & 0x80)
	{
		ep_tbl->tx.addr = pm_addr;
		ep_tbl->tx.count = 0;
		EP_TX_UNSTALL(ep_reg);
	}
	else
	{
		ep_tbl->rx.addr = pm_addr;
		set_rx_buff_size(ep, ep_max_size);
		EP_RX_UNSTALL(ep_reg);
	}
}

void process_setup_packet(uint8_t * setup_packet)
{
	usb.ep0_state = SETUP;

	usb.setup_req.type = *(setup_packet);
	usb.setup_req.request = *(setup_packet + 1);
	usb.setup_req.value = SWAPBYTE(setup_packet + 2);
	usb.setup_req.index = SWAPBYTE(setup_packet + 4);
	usb.setup_req.length = SWAPBYTE(setup_packet + 6);

	switch (usb.setup_req.type & 0x7F)
	{
	case 0x00: // device
		handle_dev_req();
		break;
	case 0x01: // interface
		handle_itf_req();
		break;
	case 0x02: // endpoint
		handle_ep_req();
		break;
	case 0x40: // vendor
		handle_vendor_req();
		break;
	default:
		ep_set_stall(0x80, 1);
		break;
	}
}

void handle_dev_req(void)
{
	switch (usb.setup_req.request)
	{
	case 0x00: // get status
		get_dev_status();
		break;
	// case 0x01: // clear feature
	// case 0x02: // reserved
	// case 0x03: // set feature
	// case 0x04: // reserved
	case 0x05: // set address
		set_address();
		break;
	case 0x06: // get descriptor
		get_dev_descriptor();
		break;
	// case 0x07: // set descriptor
	case 0x08: // get configuration
		get_dev_configuration();
		break;
	case 0x09: // set configuration
		set_dev_configuration();
		break;
	// case 0x0A: // get interface
	// case 0x0B: // set interface
	// case 0x0C: // sync frame
	default:
		ep_set_stall(0x80, 1);
		break;
	}
}

void handle_itf_req(void)
{
	uint8_t * desc = 0;
	uint16_t length = 0;

	switch (usb.setup_req.type & 0x60)
	{
	case 0x00: // Standard interface requests
		switch (usb.setup_req.request)
		{
		case 0x06: // Get descriptor
			if ((usb.setup_req.value >> 8) == 0x22)
			{
				length = HID_REPORT_DESC_LENGTH;
				desc = HID_Report_Desc;
			}
			else if ((usb.setup_req.value >> 8) == 0x21)
			{
				length = HID_DESC_LENGTH;
				desc = HID_Desc;
			}
			else
			{
				ep_set_stall(0x80, 1);
				break;
			}

			usb.ep0_state = DATA_IN;
			transmit_ep(0, desc, length);
			break;
		default:
			ep_set_stall(0x80, 1);
			break;
		}
		break;
	case 0x20: // Class interface requests
		switch (usb.setup_req.request)
		{
		// case 0x01: // Get report <-- this is the only mandatory one
		// case 0x02: // Get idle
		// case 0x09: // Set report
		case 0x0A: // Set idle
			usb.ep0_state = STATUS_IN;
			transmit_ep(0, 0, 0);
			break;
		default:
			ep_set_stall(0x80, 1);
			break;
		}
		break;
	// case 0x40: // Vendor interface requests
	default:
		ep_set_stall(0x80, 1);
		break;
	}
}

void handle_ep_req(void)
{
	switch (usb.setup_req.request)
	{
	case 0x00: // get status
		usb.ep0_state = DATA_IN;
		uint16_t stat = 0x0000;
		transmit_ep(0, (uint8_t *) &stat, 2);
		break;
	case 0x01: // clear feature
		if (usb.device_state == CONFIGURED_STATE)
		{
			usb.ep0_state = STATUS_IN;
			transmit_ep(0, 0, 0);
		}
		else
		{
			ep_set_stall(0x80, 1);
		}
		break;
	case 0x03: // set feature
		usb.ep0_state = STATUS_IN;
		transmit_ep(0, 0, 0);
		break;
	default:
		ep_set_stall(0x80, 1);
		break;
	}
}

void handle_vendor_req(void)
{
	switch (usb.setup_req.request)
	{
	case 0x00: // get VDDA voltage
		usb.ep0_state = DATA_IN;
		float vdda = read_vdda(4);
		transmit_ep(0, (uint8_t *) &vdda, 4);
		break;
	case 0x01: // get dBm
		usb.ep0_state = DATA_IN;
		uint16_t num_avgs = usb.setup_req.value;
		float dbm = read_dbm(num_avgs);
		transmit_ep(0, (uint8_t *) &dbm, 4);
		break;
	case 0x02: // set frequency
		usb.ep0_state = STATUS_IN;
		uint32_t freq = usb.setup_req.index * 65536 + usb.setup_req.value;
		// si5351_set_freq(freq, SI5351_CLK0);
		transmit_ep(0, 0, 0);
		break;
	case 0x03: // get frequency
		// uint32_t freq = si5351_get_freq(SI5351_CLK0);
		// transmit_ep(0, (uint8_t *) &freq, 4);
		break;
	case 0x04: // set attenuator
		usb.ep0_state = STATUS_IN;
		usb.setup_req.value ? set_atten(1) : set_atten(0);
		transmit_ep(0, 0, 0);
		break;
	case 0x05: // get attenuator
		usb.ep0_state = DATA_IN;
		uint8_t atten_status = get_atten();
		transmit_ep(0, &atten_status, 1);
		break;
	case 0x06: // set LEDs
		usb.ep0_state = STATUS_IN;
		set_leds(usb.setup_req.value);
		transmit_ep(0, 0, 0);
		break;
	case 0x07: // get LEDs
		usb.ep0_state = DATA_IN;
		uint8_t led_status = get_leds();
		transmit_ep(0, &led_status, 1);
		break;

	// set drive strength
	// get drive strength

	// set start freq/stop freq/step
	// get start freq/stop freq/step

	// set LEDs status
	// get LEDs status
	default:
		ep_set_stall(0x80, 1);
		break;
	}
}

void set_dev_configuration(void)
{
	uint16_t config_val = usb.setup_req.value;

	if (config_val == 0)
	{
		usb.current_config = 0;
		usb.device_state = ADDRESS_STATE;
	}
	else if (config_val == 1)
	{
		// start the interrupt in ep here
		open_ep(0x81, USB_EP_INTERRUPT, 0x0008, 0x00C0);
		usb.int_in_ep_state = IDLE;
		usb.current_config = 1;
		usb.device_state = CONFIGURED_STATE;
	}
	else
	{
		ep_set_stall(0x80, 1);
		return;
	}

	usb.ep0_state = STATUS_IN;
	transmit_ep(0, 0, 0);
}

void get_dev_configuration(void)
{
	if (usb.device_state == ADDRESS_STATE)
	{
		usb.current_config = 0;
	}
	else if (usb.device_state == DEFAULT_STATE)
	{
		ep_set_stall(0x80, 1);
		return;
	}

	usb.ep0_state = DATA_IN;
	transmit_ep(0, &(usb.current_config), 1);
}

void get_dev_status(void)
{
	uint16_t dev_status = 0x0000;
	usb.ep0_state = DATA_IN;
	transmit_ep(0, (uint8_t *) &dev_status, 2);
}

void set_address(void)
{
	uint8_t dev_addr = (uint8_t) ((usb.setup_req.value) & 0x7F);

	if (usb.device_state == CONFIGURED_STATE)
	{
		ep_set_stall(0x80, 1);
	}
	else
	{
		if (dev_addr == 0)
		{
			USB->DADDR = USB_DADDR_EF;
			usb.device_state = DEFAULT_STATE;
		}
		else
		{
			usb.device_addr_to_set = dev_addr;
			usb.device_state = ADDRESS_STATE;
		}

		usb.ep0_state = STATUS_IN;
		transmit_ep(0, 0, 0);
	}
}

void get_dev_descriptor(void)
{
	uint8_t * desc;
	uint16_t length;

	switch (usb.setup_req.value >> 8)
	{
	case 1: // device descriptor
		desc = device_desc;
		length = DEVICE_DESC_LENGTH;
		break;
	case 2: // config descriptor
		desc = config_desc;
		length = TOTAL_CONFIG_DESC_LENGTH;
		break;
	case 3: // string descriptor
		switch (usb.setup_req.value & 0x00FF)
		{
		case 0:
			desc = lang_id;
			length = 0x04;
			break;
		case 1:
			desc = manf_string;
			length = MANF_STR_LENGTH;
			break;
		case 2:
			desc = product_string;
			length = PROD_STR_LENGTH;
			break;
		default:
			ep_set_stall(0x80, 1);
			return;
		}
		break;
	case 6: // device qualifier desc
		desc = device_qual_desc;
		length = DQ_DESC_LENGTH;
		break;
	default:
		ep_set_stall(0x80, 1);
		return;
	}

	if ((length != 0) && (usb.setup_req.length != 0))
	{
		uint16_t len = MIN(length, usb.setup_req.length);

		usb.ep0_state = DATA_IN;
		transmit_ep(0x80, desc, len);
	}
}

void transmit_ep(uint8_t ep, uint8_t * buffer, uint16_t length)
{
	uint16_t * ep_reg = EPR(ep);
	pma_table * ep_tbl = EPT(ep);

	write_pm(ep_tbl->tx.addr, buffer, length);
	ep_tbl->tx.count = length;
	EP_TX_VALID(ep_reg);
}

void receive_ep(uint8_t ep, uint16_t length)
{
	uint16_t * ep_reg = EPR(ep);

	// set_rx_buff_size(ep, length);
	EP_RX_VALID(ep_reg);
}

void write_pm(uint16_t addr, uint8_t * buffer, uint16_t length)
{
	uint16_t n = (length + 1) >> 1;
	uint16_t *curr_addr = (uint16_t *) (addr + USB_PMAADDR);
	uint16_t temp1, temp2;

	for (uint16_t i = n; i != 0; i--)
	{
		temp1 = (uint16_t) *buffer;
		buffer++;
		temp2 = temp1 | ((uint16_t) ((uint16_t) *buffer << 8));
		*curr_addr++ = temp2;
		buffer++;
	}
}

void read_pm(uint16_t count, uint16_t addr, uint8_t * buffer)
{
	uint32_t n = (uint32_t) count >> 1U;
	uint16_t *curr_addr = (uint16_t *) (addr + USB_PMAADDR);
	uint32_t temp;

	for (uint32_t i = n; i != 0U; i--)
	{
		temp = *curr_addr++;
		*buffer++ = ((temp >> 0) & 0xFF);
		*buffer++ = ((temp >> 8) & 0xFF);
	}

	if (count % 2)
	{
		temp = *curr_addr++;
		*buffer++ = ((temp >> 0) & 0xFF);
	}
}

void ep_set_stall(uint8_t ep, uint8_t stall)
{
	uint16_t * ep_reg = EPR(ep);

	if (ep & 0x80)
	{
		if (stall)
			EP_TX_STALL(ep_reg);
		else
			EP_TX_UNSTALL(ep_reg);
	}
	else
	{
		if (stall)
			EP_RX_STALL(ep_reg);
		else
			EP_RX_UNSTALL(ep_reg);
	}
}

void set_rx_buff_size(uint8_t ep, uint16_t buff_size)
{
	uint16_t _count;
	pma_table * ep_tbl = EPT(ep);

	if (buff_size > 62)
	{
		_count = 0x8400; // hard-coded to buff_size of 64
	}
	else
	{
		_count = buff_size << 9; // equivalent to ((buff_size / 2) << 10)
	}

	ep_tbl->rx.count = _count;
}

uint16_t * EPR(uint8_t ep)
{
	return (uint16_t *) (USB_BASE + (ep & 0x07) * 4);
}

pma_table * EPT(uint8_t ep)
{
    return (pma_table*)((ep & 0x07) * 8 + USB_PMAADDR);
}

void USB_IRQHandler(void)
{
	if ((USB->ISTR & USB_ISTR_CTR) == USB_ISTR_CTR)
	{
		handle_ctr();
	}
	else if ((USB->ISTR & USB_ISTR_ERR) == USB_ISTR_ERR)
	{
		USB->ISTR &= ~USB_ISTR_ERR;
	}
	else if ((USB->ISTR & USB_ISTR_WKUP) == USB_ISTR_WKUP)
	{
		USB->ISTR &= ~USB_ISTR_WKUP;
	}
	else if ((USB->ISTR & USB_ISTR_SUSP) == USB_ISTR_SUSP)
	{
		USB->ISTR &= ~USB_ISTR_SUSP;
	}
	else if ((USB->ISTR & USB_ISTR_RESET) == USB_ISTR_RESET)
	{
		USB->ISTR &= ~USB_ISTR_RESET;
		reset_usb();
	}
}