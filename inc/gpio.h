#ifndef GPIO_H
#define GPIO_H

#include "stm32f0xx.h"

void gpio_init(void);
void gpio_write_pin(GPIO_TypeDef * port, uint16_t pin, uint8_t value);
uint16_t gpio_read_pin(GPIO_TypeDef * port, uint16_t pin);

#endif