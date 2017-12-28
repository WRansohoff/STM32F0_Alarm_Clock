#ifndef _VVC_UTIL_C_H
#define _VVC_UTIL_C_H

#include "global.h"

// OLED framebuffer drawing functions.
void oled_clear_screen(unsigned char color);
void oled_write_pixel(int x, int y, unsigned char color);
void oled_draw_h_line(int x, int y, int w, unsigned char color);
void oled_draw_v_line(int x, int y, int h, unsigned char color);
void oled_draw_rect(int x, int y, int w, int h,
                    int outline, unsigned char color);
void oled_draw_small_letter(int x, int y, char c, unsigned char color);
void oled_draw_small_text(int x, int y, char* cc, unsigned char color);
void oled_draw_big_letter(int x, int y, char c, unsigned char color);
void oled_draw_big_text(int x, int y, char* cc, unsigned char color);

// Alarm clock state management functions.
void process_show_time_state();
void process_in_alarm_state();
void process_menu_page_1_state();
void process_menu_page_2_state();
void process_set_time_state();
void process_set_alarm_state();
void process_set_alarm_days_state();
void process_set_alarm_tone_state();
void process_set_alarm_state_state();

#endif
