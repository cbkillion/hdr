#include "stm32f0xx.h"

void delay(int ticks);
void config_clock(void);
void init_gpio(void);

int main(void) {
    config_clock();
    init_gpio();

    // Set PA15 to be output
    // CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODER15);
    // SET_BIT(GPIOA->MODER, GPIO_MODER_MODER15_0);
 
    while (1){
        // toggle PA15
        SET_BIT(GPIOA->ODR, GPIO_ODR_15 | GPIO_ODR_14);
        delay(20000);

        CLEAR_BIT(GPIOA->ODR, GPIO_ODR_15 | GPIO_ODR_14);
        delay(20000);
    }
}

void delay(int ticks) {
    int delay = ticks;
    while(delay > 0)
        delay--;
}

void config_clock(void) {
	// enable flash prefetch buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	// setup external oscillator and wait until it's ready
	RCC->CR |= (RCC_CR_HSEBYP | RCC_CR_HSEON);
	while(!(RCC->CR & RCC_CR_HSERDY)) {}

	// disable the PLL and wait for it to turn off
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY) {}

	// modify the PLL settings
	RCC->CFGR = 0x00390000; // Set PLLMUL
	RCC->CFGR2 = 0x00000008; // Set PREDIV

	// enable the PLL and wait for it to turn on
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}

	// set flash latency to one wait state
	FLASH->ACR |= 0x00000001;

	// switch the SysClock over to the PLL
	RCC->CFGR |= 0x00000002;

	// wait for the SysClock to switch over to the PLL
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL) {}
}

void init_gpio(void) {
	// Enable the clocks for the oscillator (maybe not necessary), GPIOA, and GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN; // Ports A, B, F

	// Configure GPIOA pins 0, 14, and 15 for Button, LED, and switch control functionality
	// Configure GPIOA pin 4 for analog functionality
	GPIOA->MODER = 0x50000301; // pins 15, 14, and 0 are output, 4 is analog, all others input
	GPIOA->OTYPER = 0x00000000; // all outputs are push-pull
	GPIOA->OSPEEDR = 0xF0000003; // LEDs and switch control are high speed
	GPIOA->PUPDR = 0x00000000; // no pull-ups or pull-downs

	// Configure GPIOB pin 8 for Button (input) functionality and pins 6 and 7 for I2C
	GPIOB->MODER = 0x0000A000; // pins 6 and 7 are alternate function, all others input
	GPIOB->OTYPER = 0x000000C0; // pins 6 and 7 are open drain, all others are push-pull
	GPIOB->OSPEEDR = 0x0000F000; // pins 6 and 7 are high speed, all others are lowest speed
	GPIOB->PUPDR = 0x00000000; // no pull-ups or pull-downs
	GPIOB->AFR[0] = 0x11000000; // set pins 6 and 7 to alternate function 1 (I2C)

	GPIOF->MODER = 0x00000000;
	GPIOF->OTYPER = 0x00000000;
	GPIOF->OSPEEDR = 0x00000000;
	GPIOF->PUPDR = 0x00000000;
}

void SystemInit(void) {
	/* Reset the RCC clock configuration to the default reset state ------------*/
	/* Set HSION bit */
	RCC->CR |= (uint32_t) 0x00000001U;

	/* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
	RCC->CFGR &= (uint32_t) 0x08FFB80CU;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= (uint32_t) 0xFEF6FFFFU;

	/* Reset HSEBYP bit */
	RCC->CR &= (uint32_t) 0xFFFBFFFFU;

	/* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
	RCC->CFGR &= (uint32_t) 0xFFC0FFFFU;

	/* Reset PREDIV[3:0] bits */
	RCC->CFGR2 &= (uint32_t) 0xFFFFFFF0U;

	/* Reset USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits */
	RCC->CFGR3 &= (uint32_t) 0xFFFFFE2CU;

	/* Reset HSI14 bit */
	RCC->CR2 &= (uint32_t) 0xFFFFFFFEU;

	/* Disable all interrupts */
	RCC->CIR = 0x00000000U;
}