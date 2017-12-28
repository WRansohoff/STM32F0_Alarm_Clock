#include "main.h"

/*
 * Main program. First, run initial setup to set pin functions
 * and enable clocks. Then clear persistent values, and enter the
 * main 'alarm clock' loop.
 */
int main(void) {
    // Enable the GPIOA peripheral's clock.
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    // Enable the I2C1 peripheral's clock.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // Initialize GPIO pins 9 and 10 for I2C.
    // Set AF values. I2C1 = AF4.
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_AF;
    gpio_init_struct.GPIO_OType = GPIO_OType_OD;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init_struct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio_init_struct);

    // Initialize GPIO pins A0, A1, A2, A3 as Push/pull output.
    gpio_init_struct.GPIO_Pin   = IOA_595_CLOCK_PIN |
                                  IOA_595_DATA_PIN  |
                                  IOA_595_LATCH_PIN |
                                  IOA_BUZZER_PIN;
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init_struct.GPIO_OType = GPIO_OType_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init_struct);

    // Initialize GPIO pins A5, A6, A7 as GP input with pullups.
    gpio_init_struct.GPIO_Pin   = IOA_BUTTON_DOWN   |
                                  IOA_BUTTON_SELECT |
                                  IOA_BUTTON_UP;
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_IN;
    gpio_init_struct.GPIO_OType = 0; // N/A for input
    gpio_init_struct.GPIO_Speed = 0; // N/A for input
    gpio_init_struct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio_init_struct);

    // Initialize the I2C1 peripheral.
    i2c_periph_init(I2C1_BASE, VVC_TIMING_400KHzI2C_48MHzPLL);

    alarm_word = ds3231_get_alarm_1(I2C1_BASE);
    alarm_word = alarm_word << 8;

    // Initialize the Monochrome OLED screen.
    i2c_init_ssd1306(I2C1_BASE);

    // Initialize globals.
    time_word = 0;
    cur_state = VVC_STATE_SHOW_TIME;
    cursor_position = 0;
    last_button_state = 0;
    alarm_remember_off = 0;

    // Since this is a microcontroller, there's no point in
    // exiting our program before power-off.
    while (1) {
        // Clear the framebuffer.
        oled_clear_screen(0x00);
        // Draw an outline.
        oled_draw_rect(0, 0, 127, 63, 2, 1);

        // Get the current time.
        time_word = ds3231_get_time(I2C1_BASE);
        if ((time_word & 0x00FFFF00) == alarm_word) {
            if (!alarm_remember_off) {
                cur_state = VVC_STATE_IN_ALARM;
                cursor_position = 0;
            }
        }
        else {
            alarm_remember_off = 0;
        }

        if (cur_state == VVC_STATE_SHOW_TIME) {
            process_show_time_state();
        }
        else if (cur_state == VVC_STATE_IN_ALARM) {
            process_in_alarm_state();
        }
        else if (cur_state == VVC_STATE_MENU_PAGE_1) {
            process_menu_page_1_state();
        }
        else if (cur_state == VVC_STATE_MENU_PAGE_2) {
            process_menu_page_2_state();
        }
        else if (cur_state == VVC_STATE_SET_TIME) {
            process_set_time_state();
        }
        else if (cur_state == VVC_STATE_SET_ALARM) {
            process_set_alarm_state();
        }
        else if (cur_state == VVC_STATE_SET_ALARM_DAYS) {
            process_set_alarm_days_state();
        }
        else if (cur_state == VVC_STATE_SET_ALARM_TONE) {
            process_set_alarm_tone_state();
        }
        else if (cur_state == VVC_STATE_SET_ALARM_STATE) {
            process_set_alarm_state_state();
        }

        // Set 'last button state' value as soon after checking input
        // as possible. (Checked at the end of state methods)
        last_button_state = (~GPIOA->IDR) &
            (IOA_BUTTON_DOWN | IOA_BUTTON_SELECT | IOA_BUTTON_UP);


        // Display the framebuffer.
        i2c_display_framebuffer(I2C1_BASE, &oled_fb);

        // Delay ~500ms. But this is really really bad for input detection
        // since I'm not using hardware interrupts. So...don't delay.
        //delay_us(500000);
    }
}
