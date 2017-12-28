#ifndef _VVC_GLOBAL_H
#define _VVC_GLOBAL_H

#include "stm32f0xx.h"

// Constant value definitions.
// I2C Timing.
#define VVC_TIMING_1MHzI2C_48MHzPLL   0x50100103
#define VVC_TIMING_400KHzI2C_48MHzPLL 0x50330309
#define VVC_TIMING_100KHzI2C_48MHzPLL 0xB0420F13
#define VVC_TIMING_10KHzI2C_48MHzPLL  0xB042C3C7

// Alarm clock state.
#define VVC_STATE_SHOW_TIME        0x00
#define VVC_STATE_IN_ALARM         0x01
#define VVC_STATE_MENU_PAGE_1      0x02
#define VVC_STATE_MENU_PAGE_2      0x03
#define VVC_STATE_SET_TIME         0x04
#define VVC_STATE_SET_ALARM        0x05
#define VVC_STATE_SET_ALARM_DAYS   0x06
#define VVC_STATE_SET_ALARM_TONE   0x07
#define VVC_STATE_SET_ALARM_STATE  0x08

// 128x64-px monochrome. (1 Byte = 8 pixels)
// TODO: support 128x32-px version of the screen.
#define OLED_FB_SIZE (128*64)/8

// Pin definitions. (GPIOA)
#define IOA_595_CLOCK_PIN GPIO_Pin_0
#define IOA_595_DATA_PIN  GPIO_Pin_1
#define IOA_595_LATCH_PIN GPIO_Pin_2
#define IOA_BUZZER_PIN    GPIO_Pin_3
#define IOA_BUTTON_UP     GPIO_Pin_5
#define IOA_BUTTON_SELECT GPIO_Pin_6
#define IOA_BUTTON_DOWN   GPIO_Pin_7

// Assembly methods.
// Delay a given # of microseconds (+/- like 5-10% I guess, see src/util.S)
extern void delay_us(unsigned int d);
// Shift register output methods.
extern void shift_byte_out(unsigned char dat,
                           volatile void* gpiox_odr,
                           unsigned int clock_pinmask,
                           unsigned int data_pinmask);
extern void shift_7_segment_out(int num,
                                volatile void* gpiox_odr,
                                unsigned int clock_pinmask,
                                unsigned int data_pinmask);
extern void pulse_out_pin(volatile void* gpiox_odr,
                          unsigned int pulse_pinmask,
                          unsigned int pulse_halfw,
                          unsigned int num_pulses);
// I2C peripheral and device commands.
extern void i2c_periph_init(unsigned int i2c_addr, unsigned int i2c_speed);
extern unsigned char i2c_read_register(unsigned int i2c_register,
                                       unsigned char i2c_device_addr,
                                       unsigned char i2c_device_mem_addr);
// (SSD1306)
extern void i2c_init_ssd1306(unsigned int i2c_addr);
extern void i2c_display_framebuffer(unsigned int i2c_addr, void* fb_addr);
// (DS3231)
extern unsigned int ds3231_get_time(unsigned int i2c_addr);
extern void ds3231_set_time(unsigned int i2c_addr, int hrs_btc, int mins_btc);
extern unsigned int ds3231_get_alarm_1(unsigned int i2c_addr);
extern void ds3231_set_alarm_1_time(unsigned int i2c_addr, int hrs_btc, int mins_btc);

// Global variables/storage.
volatile unsigned char oled_fb[OLED_FB_SIZE];
volatile unsigned int time_word;
volatile unsigned int alarm_word;
volatile unsigned int time_to_set;
volatile int cur_hours;
volatile int cur_minutes;
volatile unsigned char cur_state;
volatile unsigned char cursor_position;
volatile unsigned char alarm_remember_off;
volatile unsigned int last_button_state;

#endif
