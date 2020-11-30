#include "stm32f0xx.h"

#define BUTTON_PORT     GPIOF
#define BUTT0N_PIN      GPIO_IDR_11

void configure_clock(void);
void init_gpio(void);
void write_pin(GPIO_TypeDef * port, uint16_t pin, uint8_t value);
uint16_t read_pin(GPIO_TypeDef * port, uint16_t pin);
uint16_t check_button(void);