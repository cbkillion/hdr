#include "main.h"
#include "interrupts.h"

void interrupts_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG->EXTICR[0] = 0x00000001; // Set B0, A1, A2 as external triggers
    EXTI->IMR |= 0x00000001; // Unmask line 0 for now (use 0x...7 for lines 1 and 2 also)
    EXTI->RTSR |= 0x00000006; // Risging edge trigger for pins 1 and 2
    EXTI->FTSR |= 0x00000001; // Falling edge trigger for pin 0

    NVIC_SetPriority(EXTI0_1_IRQn, 3);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler(void)
{
    // clear the interrupt flags
    EXTI->PR |= 0x00000007;

    // check the radio interrupt status, this also clears the interrupts in the radio
    si446x_command(GET_INT_STATUS, 0, 0, interrupt_status, sizeof(interrupt_status));

    // packet handler interrupts
    if (interrupt_status[0] & PACKET_HANDLER_INT)
    {
        if (interrupt_status[2] & PACKET_RECV_INT) // packet received
        {
            green_led_on();
            si446x_read_rx_fifo(rx_fifo_buffer, &rx_len);
            
            if (rx_fifo_buffer[6] == 'Z')
                green_led_off();
        }

        if (interrupt_status[2] & PACKET_SENT_INT) // packet sent
        {
            red_led_off();
        }

        if (interrupt_status[2] & CRC_ERROR_INT) // crc error...
        {
            si446x_clear_rx_fifo();
        }
    }

    // modem interrupts
    if (interrupt_status[0] & MODEM_INT)
    {
        if (interrupt_status[4] & INVALID_SYNC_INT)
        {
            si446x_clear_rx_fifo();
        }
    }

    // chip interrupts
    // if (interrupt_status[0] & CHIP_INT)
}

void EXTI2_3_IRQHandler(void)
{

}