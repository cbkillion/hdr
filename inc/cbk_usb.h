#ifndef CBK_USB_H_
#define CBK_USB_H_

#include "stm32f0xx.h"

#define VID	0xF055
#define PID 0x1134
#define EP0_MAX_SIZE 0x0040

#define DEVICE_DESC_LENGTH 0x12
#define CONFIG_DESC_LENGTH 0x09
#define TOTAL_CONFIG_DESC_LENGTH 0x22
#define INTF_DESC_LENGTH 0x09
#define HID_DESC_LENGTH 0x09
#define HID_REPORT_DESC_LENGTH 0x16
#define DQ_DESC_LENGTH 0x0A
#define HID_REPORT_SIZE 24

#define MANF_STR_LENGTH 0x28
#define PROD_STR_LENGTH 0x3A

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define SWAPBYTE(addr) (((uint16_t)(*((uint8_t *)(addr)))) + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define EP_TOGGLE_SET(ep_reg, bits, mask) *(ep_reg) = (*(ep_reg) ^ (bits)) & (USB_EPREG_MASK | (mask))

#define EP_TX_STALL(ep_reg)    EP_TOGGLE_SET((ep_reg), USB_EP_TX_STALL, USB_EPTX_STAT)
#define EP_RX_STALL(ep_reg)    EP_TOGGLE_SET((ep_reg), USB_EP_RX_STALL, USB_EPRX_STAT)
#define EP_TX_UNSTALL(ep_reg)  EP_TOGGLE_SET((ep_reg), USB_EP_TX_NAK,   USB_EPTX_STAT | USB_EP_DTOG_TX)
#define EP_RX_UNSTALL(ep_reg)  EP_TOGGLE_SET((ep_reg), USB_EP_RX_VALID, USB_EPRX_STAT | USB_EP_DTOG_RX)
#define EP_TX_VALID(ep_reg)    EP_TOGGLE_SET((ep_reg), USB_EP_TX_VALID, USB_EPTX_STAT)
#define EP_RX_VALID(ep_reg)    EP_TOGGLE_SET((ep_reg), USB_EP_RX_VALID, USB_EPRX_STAT)

#define EP_CLEAR_CTR_TX(ep_reg) *(ep_reg) &= (USB_EPREG_MASK ^ USB_EP_CTR_TX)
#define EP_CLEAR_CTR_RX(ep_reg) *(ep_reg) &= (USB_EPREG_MASK ^ USB_EP_CTR_RX)

typedef enum
{
	DEFAULT_STATE,
	ADDRESS_STATE,
	CONFIGURED_STATE,
	SUSPENDED_STATE
} DeviceStateTypeDef;

typedef enum
{
	IDLE,
	SETUP,
	DATA_IN,
	DATA_OUT,
	STATUS_IN,
	STATUS_OUT,
	STALL
} EPStateTypeDef;

typedef struct
{
	uint8_t type;
	uint8_t request;
	uint16_t value;
	uint16_t index;
	uint16_t length;
} SetupReqTypedef;

typedef struct
{
	EPStateTypeDef ep0_state;
	DeviceStateTypeDef device_state;
	uint8_t device_addr_to_set;
	SetupReqTypedef setup_req;
	uint8_t current_config;
	EPStateTypeDef int_in_ep_state;
} USBInfoTypeDef;

typedef struct
{
	EPStateTypeDef state;
	uint32_t total_length;
	uint32_t rem_length;
	uint32_t maxpacket;
} EndpointTypeDef;

typedef struct
{
    uint16_t    addr;
    uint16_t    count;
} pma_record;

typedef struct pma_table
{
    pma_record     tx;
    pma_record     rx;
} pma_table;

typedef struct
{
    double adc_reading;
    double frequency;
    double something;
} HIDReportTypeDef;

void init_usb(void);
void reset_usb(void);
void handle_ctr(void);

void open_ep(uint8_t addr, uint16_t ep_type, uint16_t ep_max_size, uint16_t pm_addr);
void set_rx_buff_size(uint8_t ep_num, uint16_t buff_size);

uint16_t * EPR(uint8_t ep);
pma_table * EPT(uint8_t ep);

void ep_set_stall(uint8_t ep_num, uint8_t stall);

void transmit_ep(uint8_t ep_num, uint8_t * buffer, uint16_t length);
void receive_ep(uint8_t ep_num, uint16_t length);

void read_pm(uint16_t count, uint16_t addr, uint8_t * buffer);
void write_pm(uint16_t addr, uint8_t * buffer, uint16_t length);

void process_setup_packet(uint8_t * setup_packet);
void in_transaction_callback(uint8_t ep_num);
void out_transaction_callback(uint8_t ep_num);

void handle_dev_req(void);
void handle_itf_req(void);
void handle_ep_req(void);
void handle_vendor_req(void);

void get_dev_descriptor(void);
void set_dev_configuration(void);
void get_dev_configuration(void);
void set_address(void);
void get_dev_status(void);

void USB_IRQHandler(void);

#endif
