#include "stm32f0xx.h"

int main(void)
{
	RCC->AHBENR 	= 0x00020000;
	GPIOA->MODER 	= 0x50000000;
	GPIOA->ODR 		= 0x00008000;
 
    while (1) {}
}