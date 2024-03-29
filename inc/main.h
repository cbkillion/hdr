#ifndef MAIN_H
#define MAIN_H

#include "stm32f0xx.h"
#include "gpio.h"
#include "spi.h"
#include "interrupts.h"
#include "si446x.h"

#define SDN_PORT        GPIOA
#define SDN_PIN         3
#define NSS_PORT        GPIOA
#define NSS_PIN         4
#define CTS_PORT        GPIOB
#define CTS_PIN         1

#define BUTTON_PORT     GPIOB
#define BUTT0N_PIN      8
#define RED_LED_PORT    GPIOB
#define RED_LED_PIN     6
#define GREEN_LED_PORT  GPIOB
#define GREEN_LED_PIN   7

#define nIRQ_PORT       GPIOB
#define nIRQ_PIN        0

#define ON              1
#define OFF             0

uint8_t interrupt_status[8];
uint8_t rx_buffer[64];
uint8_t tx_buffer[64];
uint8_t new_rx_data;

void configure_clock(void);
void gpio_init(void);

void write_pin(GPIO_TypeDef * port, uint16_t pin, uint8_t value);
uint16_t read_pin(GPIO_TypeDef * port, uint16_t pin);
uint16_t read_button(void);
void delay(uint32_t delay);

void red_led(uint8_t state);
void green_led(uint8_t state);

void nss_low(void);
void nss_high(void);

void flash_led(uint8_t num_flashes);

#endif