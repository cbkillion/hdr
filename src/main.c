#include "stm32f0xx.h"

int main(void)
{
	RCC->AHBENR 	= 0x00040000;
	GPIOB->MODER 	= 0x00005000;
	GPIOB->ODR 		= 0x000000C0;
 
    while (1) {}
}