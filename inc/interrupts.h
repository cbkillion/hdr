#ifndef INT_H
#define INT_H

#include "stm32f0xx.h"

void interrupts_init(void);

void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);

#endif