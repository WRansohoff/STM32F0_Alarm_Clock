#include "util_c.h"

/*
 * Clear the OLED display to a set color byte.
 * 0x00 = off,
 * 0xFF = primary color 'on'.
 * But technically, you could use any value and get a pattern
 * of horizontal lines repeating every 8 rows. 
 */
void oled_clear_screen(unsigned char color) {
    int i;
    for (i = 0; i < OLED_FB_SIZE; ++i) {
        oled_fb[i] = color;
    }
}

/*
 * Write a pixel in the current OLED framebuffer.
 * Note that the positioning is a bit odd; each byte is a VERTICAL column
 * of 8 pixels, but each successive byte increments the row position by 1.
 * This means that the buffer is 8x 128-byte pages stacked on top of one
 * another. To set an (x, y) pixel, we |= one position in one byte.
 *   Byte offset = x + ((y / 8) * 128)
 *   Bit offset  = (y & 0x07)
 * 'color' indicates whether to set or unset the pixel. 0 means 'unset.'
 */
void oled_write_pixel(int x, int y, unsigned char color) {
    int y_page = y / 8;
    int byte_to_mod = x + (y_page * 128);
    int bit_to_set = 0x01 << (y & 0x07);
    if (color) {
        oled_fb[byte_to_mod] |= bit_to_set;
    }
    else {
        bit_to_set = ~bit_to_set;
        oled_fb[byte_to_mod] &= bit_to_set;
    }
}

/*
 * Draw a horizontal line.
 * First, calculate the Y bitmask and byte offset, then just go from x->x.
 */
void oled_draw_h_line(int x, int y, int w, unsigned char color) {
    int y_page_offset = y / 8;
    y_page_offset *= 128;
    int bit_to_set = 0x01 << (y & 0x07);
    if (!color) {
        bit_to_set = ~bit_to_set;
    }
    int x_pos;
    for (x_pos = x; x_pos < (x+w); ++x_pos) {
        if (color) {
            oled_fb[x_pos + y_page_offset] |= bit_to_set;
        }
        else {
            oled_fb[x_pos + y_page_offset] &= bit_to_set;
        }
    }
}

/*
 * Draw a veritcal line.
 */
void oled_draw_v_line(int x, int y, int h, unsigned char color) {
    int y_page_offset;
    int bit_to_set;
    int y_pos;
    for (y_pos = y; y_pos < (y+h); ++y_pos) {
        y_page_offset = y_pos/8;
        y_page_offset *= 128;
        bit_to_set = 0x01 << (y_pos & 0x07);
        if (color) {
            oled_fb[x + y_page_offset] |= bit_to_set;
        }
        else {
            bit_to_set = ~bit_to_set;
            oled_fb[x + y_page_offset] &= bit_to_set;
        }
    }
}

/*
 * Draw a rectangle on the display.
 * I guess just pick the longer dimension, and either draw
 * horizontal or vertical lines.
 * Notable args:
 *   - outline: If <=0, fill the rectangle with 'color'.
 *              If >0, draw an outline inside the dimensions of N pixels.
 *   - color: If 0, clear drawn bits. If not 0, set drawn bits.
 */
void oled_draw_rect(int x, int y, int w, int h,
                    int outline, unsigned char color) {
    if (outline > 0) {
        // Draw an outline.
        int o_pos;
        // Top.
        for (o_pos = y; o_pos < (y+outline); ++o_pos) {
            oled_draw_h_line(x, o_pos, w, color);
        }
        // Bottom.
        for (o_pos = (y+h-1); o_pos > (y+h-1-outline); --o_pos) {
            oled_draw_h_line(x, o_pos, w, color);
        }
        // Left.
        for (o_pos = x; o_pos < (x+outline); ++o_pos) {
            oled_draw_v_line(o_pos, y, h, color);
        }
        // Right.
        for (o_pos = (x+w-1); o_pos > (x+w-1-outline); --o_pos) {
            oled_draw_v_line(o_pos, y, h, color);
        }
    }
    else {
        // Draw a filled rectangle.
        if (w > h) {
            // Draw fewer horizontal lines than vertical ones.
            int y_pos;
            for (y_pos = y; y_pos < (y+h); ++y_pos) {
                oled_draw_h_line(x, y_pos, w, 1);
            }
        }
        else {
            // Draw fewer (or ==) vertical lines than horizontal ones.
            int x_pos;
            for (x_pos = x; x_pos < (x+w); ++x_pos) {
                oled_draw_v_line(x_pos, y, h, 1);
            }
        }
    }
}

/*
 * Draw a small letter. Each one is 5px wide (+1px for a space) and 8px
 * tall. So if y%8==0 they can easily fit in 1.5 words.
 * But for now, just write a series of lines and pixels for most letters.
 * Again, to save code space, only use letters that are...well, used.
 */
void oled_draw_small_letter(int x, int y, char c, unsigned char color) {
    if (c == 'A') {
        oled_draw_v_line(x, y+3, 5, 1);
        oled_draw_v_line(x+4, y+3, 5, 1);
        oled_draw_v_line(x+1, y+1, 2, 1);
        oled_draw_v_line(x+3, y+1, 2, 1);
        oled_write_pixel(x+2, y, 1);
        oled_draw_h_line(x+1, y+4, 3, 1);
    }
    else if (c == 'D') {
        oled_draw_v_line(x, y, 8, 1);
        oled_draw_v_line(x+4, y+1, 6, 1);
        oled_draw_h_line(x+1, y, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
    }
    else if (c == 'E') {
        oled_draw_v_line(x, y, 8, 1);
        oled_draw_h_line(x+1, y, 4, 1);
        oled_draw_h_line(x+1, y+7, 4, 1);
        oled_draw_h_line(x+1, y+4, 3, 1);
    }
    else if (c == 'M') {
        oled_draw_v_line(x, y, 8, 1);
        oled_draw_v_line(x+4, y, 8, 1);
        oled_draw_v_line(x+2, y+2, 2, 1);
        oled_write_pixel(x+1, y+1, 1);
        oled_write_pixel(x+3, y+1, 1);
    }
    else if (c == 'O') {
        oled_draw_v_line(x, y+1, 6, 1);
        oled_draw_v_line(x+4, y+1, 6, 1);
        oled_draw_h_line(x+1, y, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
    }
    else if (c == 'S') {
        oled_draw_v_line(x, y+1, 2, 1);
        oled_draw_v_line(x, y+5, 2, 1);
        oled_draw_v_line(x+4, y+1, 2, 1);
        oled_draw_v_line(x+4, y+5, 2, 1);
        oled_draw_h_line(x+1, y, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
        oled_draw_h_line(x+1, y+3, 2, 1);
        oled_draw_h_line(x+2, y+4, 2, 1);
    }
    else if (c == 'T') {
        oled_draw_h_line(x, y, 5, 1);
        oled_draw_v_line(x+2, y, 8, 1);
    }
    else if (c == 'a') {
        oled_draw_h_line(x+1, y+2, 3, 1);
        oled_draw_h_line(x+1, y+4, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
        oled_draw_v_line(x, y+5, 2, 1);
        oled_draw_v_line(x+4, y+3, 4, 1);
    }
    else if (c == 'e') {
        oled_draw_h_line(x+1, y+1, 3, 1);
        oled_draw_h_line(x+1, y+4, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
        oled_write_pixel(x+4, y+6, 1);
        oled_draw_v_line(x, y+2, 5, 1);
        oled_draw_v_line(x+4, y+2, 3, 1);
    }
    else if (c == 'f') {
        oled_draw_h_line(x+2, y, 2, 1);
        oled_draw_h_line(x, y+4, 4, 1);
        oled_draw_v_line(x+1, y+1, 7, 1);
        oled_draw_v_line(x+4, y+1, 2, 1);
    }
    else if (c == 'i') {
        oled_write_pixel(x+2, y+1, 1);
        oled_draw_v_line(x+2, y+3, 5, 1);
    }
    else if (c == 'l') {
        oled_draw_v_line(x+2, y, 8, 1);
    }
    else if (c == 'm') {
        oled_draw_h_line(x+1, y+3, 3, 1);
        oled_draw_v_line(x, y+2, 6, 1);
        oled_draw_v_line(x+2, y+3, 5, 1);
        oled_draw_v_line(x+4, y+3, 5, 1);
    }
    else if (c == 'n') {
        oled_draw_h_line(x+1, y+3, 2, 1);
        oled_draw_v_line(x, y+2, 6, 1);
        oled_draw_v_line(x+3, y+4, 4, 1);
    }
    else if (c == 'o') {
        oled_draw_h_line(x+1, y+3, 3, 1);
        oled_draw_h_line(x+1, y+7, 3, 1);
        oled_draw_v_line(x, y+4, 3, 1);
        oled_draw_v_line(x+4, y+4, 3, 1);
    }
    else if (c == 'r') {
        oled_draw_h_line(x+2, y+3, 2, 1);
        oled_write_pixel(x+4, y+4, 1);
        oled_draw_v_line(x+1, y+2, 6, 1);
    }
    else if (c == 's') {
        oled_draw_h_line(x+2, y+1, 2, 1);
        oled_draw_h_line(x+2, y+4, 2, 1);
        oled_draw_h_line(x+2, y+7, 2, 1);
        oled_write_pixel(x+1, y+6, 1);
        oled_write_pixel(x+4, y+2, 1);
        oled_draw_v_line(x+1, y+2, 2, 1);
        oled_draw_v_line(x+4, y+5, 2, 1);
    }
    else if (c == 't') {
        oled_draw_h_line(x, y+2, 4, 1);
        oled_draw_h_line(x+2, y+7, 2, 1);
        oled_write_pixel(x+4, y+6, 1);
        oled_draw_v_line(x+1, y, 7, 1);
    }
    else if (c == 'u') {
        oled_draw_h_line(x+1, y+6, 4, 1);
        oled_write_pixel(x+4, y+7, 1);
        oled_draw_v_line(x, y+2, 4, 1);
        oled_draw_v_line(x+3, y+2, 4, 1);
    }
    else if (c == 'x') {
        oled_write_pixel(x, y+3, 1);
        oled_write_pixel(x, y+7, 1);
        oled_write_pixel(x+1, y+4, 1);
        oled_write_pixel(x+1, y+6, 1);
        oled_write_pixel(x+2, y+5, 1);
        oled_write_pixel(x+3, y+4, 1);
        oled_write_pixel(x+3, y+6, 1);
        oled_write_pixel(x+4, y+3, 1);
        oled_write_pixel(x+4, y+7, 1);
    }
    else if (c == 'y') {
        oled_draw_h_line(x+1, y+4, 2, 1);
        oled_draw_h_line(x+1, y+7, 2, 1);
        oled_write_pixel(x, y+6, 1);
        oled_draw_v_line(x, y+2, 2, 1);
        oled_draw_v_line(x+3, y+2, 5, 1);
    }
    else if (c == ':') {
        oled_write_pixel(x+2, y+2, 1);
        oled_write_pixel(x+2, y+5, 1);
    }
    else if (c == '/') {
        oled_draw_v_line(x+1, y+5, 2, 1);
        oled_draw_v_line(x+2, y+3, 2, 1);
        oled_draw_v_line(x+3, y+1, 2, 1);
    }
    else if (c == '>') {
        oled_write_pixel(x+1, y+1, 1);
        oled_write_pixel(x+2, y+2, 1);
        oled_write_pixel(x+3, y+3, 1);
        oled_write_pixel(x+2, y+4, 1);
        oled_write_pixel(x+1, y+5, 1);
    }
    else if (c == ' ') {
    }
}

/*
 * Draw a line of small text. Does not do text wrapping.
 */
void oled_draw_small_text(int x, int y, char* cc, unsigned char color) {
    int i = 0;
    int offset = 0;
    while (cc[i] != '\0') {
        oled_draw_small_letter(x + offset, y, cc[i], color);
        offset += 6;
        ++i;
    }
}

/*
 * Draw a large letter. These are about 18px monospace.
 * I only added the subset of letters needed, to save code space.
 */
void oled_draw_big_letter(int x, int y, char c, unsigned char color) {
    if (c == 'A') {
        oled_draw_rect(x, y+11, 2, 2, 0, color);
        oled_draw_rect(x+7, y+11, 2, 2, 0, color);
        oled_draw_rect(x+1, y+8, 2, 3, 0, color);
        oled_draw_rect(x+1, y+8, 7, 2, 0, color);
        oled_draw_rect(x+6, y+8, 2, 3, 0, color);
        oled_draw_rect(x+2, y+4, 2, 4, 0, color);
        oled_draw_rect(x+5, y+4, 2, 4, 0, color);
        oled_draw_rect(x+3, y, 3, 4, 0, color);
    }
    else if (c == 'D') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+2, y, 4, 2, 0, color);
        oled_draw_rect(x+2, y+11, 4, 2, 0, color);
        oled_draw_rect(x+5, y+1, 2, 2, 0, color);
        oled_draw_rect(x+6, y+2, 2, 2, 0, color);
        oled_draw_rect(x+7, y+3, 2, 7, 0, color);
        oled_draw_rect(x+5, y+10, 2, 2, 0, color);
        oled_draw_rect(x+6, y+9, 2, 2, 0, color);
    }
    else if (c == 'E') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+2, y, 7, 2, 0, color);
        oled_draw_rect(x+2, y+5, 7, 2, 0, color);
        oled_draw_rect(x+2, y+11, 7, 2, 0, color);
    }
    else if (c == 'I') {
        oled_draw_rect(x, y, 8, 2, 0, color);
        oled_draw_rect(x+3, y+2, 2, 9, 0, color);
        oled_draw_rect(x, y+11, 8, 2, 0, color);
    }
    else if (c == 'L') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+2, y+11, 7, 2, 0, color);
    }
    else if (c == 'M') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+2, y, 1, 4, 0, color);
        oled_draw_rect(x+6, y, 1, 4, 0, color);
        oled_draw_rect(x+3, y+3, 1, 4, 0, color);
        oled_draw_rect(x+5, y+3, 1, 4, 0, color);
        oled_draw_rect(x+4, y+5, 1, 3, 0, color);
        oled_draw_rect(x+7, y, 2, 13, 0, color);
    }
    else if (c == 'N') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+7, y, 2, 13, 0, color);
        oled_draw_v_line(x+2, y, 3, color);
        oled_draw_v_line(x+3, y+2, 4, color);
        oled_draw_v_line(x+4, y+5, 3, color);
        oled_draw_v_line(x+5, y+7, 4, color);
        oled_draw_v_line(x+6, y+10, 3, color);
    }
    else if (c == 'O') {
        oled_draw_rect(x, y+3, 2, 7, 0, color);
        oled_draw_rect(x+7, y+3, 2, 7, 0, color);
        oled_draw_rect(x+2, y, 5, 2, 0, color);
        oled_draw_rect(x+2, y+11, 5, 2, 0, color);
        oled_draw_rect(x+1, y+1, 2, 2, 0, color);
        oled_draw_rect(x+1, y+10, 2, 2, 0, color);
        oled_draw_rect(x+6, y+1, 2, 2, 0, color);
        oled_draw_rect(x+6, y+10, 2, 2, 0, color);
    }
    else if (c == 'R') {
        oled_draw_rect(x, y, 2, 13, 0, color);
        oled_draw_rect(x+2, y, 5, 2, 0, color);
        oled_draw_rect(x+6, y+1, 2, 2, 0, color);
        oled_draw_rect(x+6, y+5, 2, 2, 0, color);
        oled_draw_rect(x+6, y+8, 2, 2, 0, color);
        oled_draw_rect(x+2, y+6, 5, 2, 0, color);
        oled_draw_rect(x+7, y+2, 2, 4, 0, color);
        oled_draw_rect(x+7, y+9, 2, 4, 0, color);
    }
    else if (c == 'S') {
        oled_draw_rect(x+2, y, 6, 2, 0, color);
        oled_draw_rect(x+1, y+1, 2, 2, 0, color);
        oled_draw_rect(x+1, y+4, 2, 2, 0, color);
        oled_draw_rect(x, y+2, 2, 3, 0, color);
        oled_draw_rect(x+2, y+5, 5, 2, 0, color);
        oled_draw_rect(x+6, y+6, 2, 2, 0, color);
        oled_draw_rect(x+6, y+10, 2, 2, 0, color);
        oled_draw_rect(x+7, y+7, 2, 4, 0, color);
        oled_draw_rect(x, y+11, 7, 2, 0, color);
    }
    else if (c == 'T') {
        oled_draw_rect(x, y, 9, 2, 0, color);
        oled_draw_rect(x+4, y+2, 2, 11, 0, color);
    }
    else if (c == 'U') {
        oled_draw_rect(x, y, 2, 11, 0, color);
        oled_draw_rect(x+7, y, 2, 11, 0, color);
        oled_draw_rect(x+1, y+10, 2, 2, 0, color);
        oled_draw_rect(x+6, y+10, 2, 2, 0, color);
        oled_draw_rect(x+2, y+11, 5, 2, 0, color);
    }
    else if (c == 'Y') {
        oled_draw_rect(x, y, 2, 3, 0, color);
        oled_draw_rect(x+6, y, 2, 3, 0, color);
        oled_draw_rect(x+1, y+2, 2, 2, 0, color);
        oled_draw_rect(x+5, y+2, 2, 2, 0, color);
        oled_draw_rect(x+2, y+4, 4, 2, 0, color);
        oled_draw_rect(x+3, y+6, 2, 7, 0, color);
    }
    else if (c == ':') {
        oled_draw_rect(x, y+2, 3, 3, 0, color);
        oled_draw_rect(x, y+8, 3, 3, 0, color);
    }
    else if (c == '?') {
        oled_draw_rect(x+1, y, 6, 2, 0, color);
        oled_draw_rect(x+6, y+1, 2, 4, 0, color);
        oled_draw_rect(x+5, y+4, 2, 2, 0, color);
        oled_draw_rect(x+4, y+5, 2, 2, 0, color);
        oled_draw_rect(x+3, y+6, 2, 4, 0, color);
        oled_draw_rect(x+3, y+11, 2, 2, 0, color);
    }
    else if (c == '!') {
        oled_draw_rect(x+4, y, 2, 9, 0, color);
        oled_draw_rect(x+4, y+11, 2, 2, 0, color);
    }
}

/*
 * Draw a line of big text. Does not do text wrapping.
 */
void oled_draw_big_text(int x, int y, char* cc, unsigned char color) {
    int i = 0;
    int offset = 0;
    while (cc[i] != '\0') {
        oled_draw_big_letter(x + offset, y, cc[i], color);
        offset += 11;
        ++i;
    }
}

/*
 * Process the default 'show time' clock state.
 */
void process_show_time_state() {
    // Draw the OLED GUI. Just print, "TIME:" in the center.
    char time_buffer[6] = { 'T', 'I', 'M', 'E', ':', '\0' };
    oled_draw_big_text(37, 26, time_buffer, 1);

    // Pull the current latch pin low.
    GPIOA->ODR &= ~IOA_595_LATCH_PIN;
    // Write the current time to the 7-segment displays.
    // Minutes 'ones'.
    int cur_digit = (time_word & 0x00000F00) >> 8;
    shift_7_segment_out(cur_digit, &GPIOA->ODR,
                        IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    // Minutes 'tens'.
    cur_digit = (time_word & 0x0000F000) >> 12;
    shift_7_segment_out(cur_digit, &GPIOA->ODR,
                        IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    // Hours 'ones'.
    cur_digit = (time_word & 0x000F0000) >> 16;
    shift_7_segment_out(cur_digit, &GPIOA->ODR,
                        IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    // 'Hours tens'.
    cur_digit = (time_word & 0x00300000) >> 20;
    shift_7_segment_out(cur_digit, &GPIOA->ODR,
                        IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    // Pull the latch pin high.
    GPIOA->ODR |= IOA_595_LATCH_PIN;

    // Check input.
    // Up/Down buttons do nothing.
    // If Select button is pressed, switch to the 'menu page 1' state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        cur_state = VVC_STATE_MENU_PAGE_1;
    }
}

/*
 * Process the 'ALARM IS GOING OFF!!!!!' state.
 */
void process_in_alarm_state() {
    // Draw centered 'ALARM!!!!'
    char alarm_buffer[10] = { 'A', 'L', 'A', 'R', 'M',
                              '!', '!', '!', '!', '\0' };
    oled_draw_big_text(18, 26, alarm_buffer, 1);

    // Pull the current latch pin low.
    GPIOA->ODR &= ~IOA_595_LATCH_PIN;
    int cur_digit = 0xFF;
    if (time_word & 0x00000001) {
        // Blank digits.
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        // Pull the latch pin high.
        GPIOA->ODR |= IOA_595_LATCH_PIN;
    }
    else {
        // Show digits, alarm.
        // Minutes 'ones'.
        cur_digit = (time_word & 0x00000F00) >> 8;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        // Minutes 'tens'.
        cur_digit = (time_word & 0x0000F000) >> 12;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        // Hours 'ones'.
        cur_digit = (time_word & 0x000F0000) >> 16;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        // 'Hours tens'.
        cur_digit = (time_word & 0x00300000) >> 20;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        // Pull the latch pin high.
        GPIOA->ODR |= IOA_595_LATCH_PIN;
        pulse_out_pin(&GPIOA->ODR, IOA_BUZZER_PIN, 200, 500);
    }

    // Check input.
    // Up/Down buttons do nothing.
    // If Select button is pressed, switch to the 'show time' state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        cur_state = VVC_STATE_SHOW_TIME;
        alarm_remember_off = 1;
    }
}

/*
 * Process the 'menu page 1' state.
 */
void process_menu_page_1_state() {
    // Draw a large, 'MENU' along the top.
    char menu_buffer[5] = { 'M', 'E', 'N', 'U', '\0' };
    oled_draw_big_text(42, 4, menu_buffer, 1);

    // Draw 3 menu lines; 'Set Time', 'Set Alarm', 'Set Alarm Days'.
    oled_draw_h_line(0, 18, 127, 1);
    char set_time_buffer[9] = { 'S', 'e', 't', ' ', 
                                'T', 'i', 'm', 'e', '\0' };
    oled_draw_small_text(72, 20, set_time_buffer, 1);
    oled_draw_h_line(0, 30, 127, 1);
    char set_alarm_buffer[10] = { 'S', 'e', 't', ' ', 'A',
                                  'l', 'a', 'r', 'm', '\0' };
    oled_draw_small_text(68, 32, set_alarm_buffer, 1);
    oled_draw_h_line(0, 42, 127, 1);
    char set_alarm_days_buffer[15] = { 'S', 'e', 't', ' ', 'A',
                                       'l', 'a', 'r', 'm', ' ',
                                       'D', 'a', 'y', 's', '\0'};
    oled_draw_small_text(40, 44, set_alarm_days_buffer, 1);
    oled_draw_h_line(0, 54, 127, 1);

    // Draw a chevron at the appropriate height.
    oled_draw_small_letter(12, 21+(cursor_position*12), '>', 1);

    // Check input.
    // Up/Down buttons change menu cursor position.
    if (((~GPIOA->IDR) & IOA_BUTTON_UP) &&
        !(last_button_state & IOA_BUTTON_UP)) {
        if (cursor_position > 0) { --cursor_position; }
    }
    else if (((~GPIOA->IDR) & IOA_BUTTON_DOWN) &&
             !(last_button_state & IOA_BUTTON_DOWN)) {
        if (cursor_position < 2) { ++cursor_position; }
        else {
            // Move to menu page 2, and set cursor position to 0.
            cur_state = VVC_STATE_MENU_PAGE_2;
            cursor_position = 0;
        }
    }
    // If Select button is pressed, switch to the selected state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        if (cursor_position == 0) {
            cur_state = VVC_STATE_SET_TIME;
            // Set 'time_to_set' to the current time.
            time_to_set = time_word;
            int hours_tens = (time_to_set & 0x00300000) >> 20;
            int hours_ones = (time_to_set & 0x000F0000) >> 16;
            cur_hours = (hours_tens * 10) + hours_ones;
            int minutes_tens = (time_to_set & 0x0000F000) >> 12;
            int minutes_ones = (time_to_set & 0x00000F00) >> 8;
            cur_minutes = (minutes_tens * 10) + minutes_ones;
        }
        else if (cursor_position == 1) {
            cur_state = VVC_STATE_SET_ALARM;
            cursor_position = 0;
            // Set the 'time_to_set' values to the current 'alarm 1'.
            time_to_set = ds3231_get_alarm_1(I2C1_BASE);
            int hours_tens = (time_to_set & 0x00003000) >> 12;
            int hours_ones = (time_to_set & 0x00000F00) >> 8;
            cur_hours = (hours_tens * 10) + hours_ones;
            int minutes_tens = (time_to_set & 0x000000F0) >> 4;
            int minutes_ones = (time_to_set & 0x0000000F);
            cur_minutes = (minutes_tens * 10) + minutes_ones;
        }
        else if (cursor_position == 2) {
            cur_state = VVC_STATE_SET_ALARM_DAYS;
            cursor_position = 0;
            // Set the 'time_to_set' values to the current 'alarm 1'.
            time_to_set = ds3231_get_alarm_1(I2C1_BASE);
            int hours_tens = (time_to_set & 0x00003000) >> 12;
            int hours_ones = (time_to_set & 0x00000F00) >> 8;
            cur_hours = (hours_tens * 10) + hours_ones;
            int minutes_tens = (time_to_set & 0x000000F0) >> 4;
            int minutes_ones = (time_to_set & 0x0000000F);
            cur_minutes = (minutes_tens * 10) + minutes_ones;
        }
        else {
            cur_state = VVC_STATE_SHOW_TIME; // (Shouldn't happen)
        }
        // Also reset cursor position to 0.
        cursor_position = 0;
    }
}

/*
 * Process the 'menu page 2' state.
 */
void process_menu_page_2_state() {
    // Draw a large, 'MENU' along the top.
    char menu_buffer[5] = { 'M', 'E', 'N', 'U', '\0' };
    oled_draw_big_text(42, 4, menu_buffer, 1);

    // Draw 3 menu lines:
    // 'Set Alarm On/Off', 'Set Alarm Tone', 'Exit Menu'.
    oled_draw_h_line(0, 18, 127, 1);
    char set_alarm_state_buffer[17] = { 'S', 'e', 't', ' ', 'A',
                                        'l', 'a', 'r', 'm', ' ',
                                        'O', 'n', '/', 'O', 'f',
                                        'f', '\0' };
    oled_draw_small_text(28, 20, set_alarm_state_buffer, 1);
    oled_draw_h_line(0, 30, 127, 1);
    char set_alarm_tone_buffer[15] = { 'S', 'e', 't', ' ', 'A',
                                     'l', 'a', 'r', 'm', ' ',
                                     'T', 'o', 'n', 'e', '\0'};
    oled_draw_small_text(38, 32, set_alarm_tone_buffer, 1);
    oled_draw_h_line(0, 42, 127, 1);
    char exit_menu_buffer[10] = { 'E', 'x', 'i', 't', ' ',
                                'M', 'e', 'n', 'u', '\0' };
    oled_draw_small_text(66, 44, exit_menu_buffer, 1);
    oled_draw_h_line(0, 54, 127, 1);

    // Draw a chevron at the appropriate height.
    oled_draw_small_letter(12, 21+(cursor_position*12), '>', 1);

    // Check input.
    // Up/Down buttons change menu cursor position.
    if (((~GPIOA->IDR) & IOA_BUTTON_UP) &&
        !(last_button_state & IOA_BUTTON_UP)) {
        if (cursor_position > 0) { --cursor_position; }
        else {
            // Move to menu page 1, and set cursor position to 0.
            cur_state = VVC_STATE_MENU_PAGE_1;
            cursor_position = 0;
        }
    }
    else if (((~GPIOA->IDR) & IOA_BUTTON_DOWN) &&
             !(last_button_state & IOA_BUTTON_DOWN)) {
        if (cursor_position < 2) { ++cursor_position; }
    }
    // If Select button is pressed, switch to the selected state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        if (cursor_position == 0) {
            cur_state = VVC_STATE_SET_ALARM_STATE;
            // Set the 'time_to_set' values to the current 'alarm 1'.
            time_to_set = ds3231_get_alarm_1(I2C1_BASE);
            int hours_tens = (time_to_set & 0x00003000) >> 12;
            int hours_ones = (time_to_set & 0x00000F00) >> 8;
            cur_hours = (hours_tens * 10) + hours_ones;
            int minutes_tens = (time_to_set & 0x000000F0) >> 4;
            int minutes_ones = (time_to_set & 0x0000000F);
            cur_minutes = (minutes_tens * 10) + minutes_ones;
        }
        else if (cursor_position == 1) {
            cur_state = VVC_STATE_SET_ALARM_TONE;
            cursor_position = 0;
            // Set the 'time_to_set' values to the current 'alarm 1'.
            time_to_set = ds3231_get_alarm_1(I2C1_BASE);
            int hours_tens = (time_to_set & 0x00003000) >> 12;
            int hours_ones = (time_to_set & 0x00000F00) >> 8;
            cur_hours = (hours_tens * 10) + hours_ones;
            int minutes_tens = (time_to_set & 0x000000F0) >> 4;
            int minutes_ones = (time_to_set & 0x0000000F);
            cur_minutes = (minutes_tens * 10) + minutes_ones;
        }
        else {
            // (Covers 'Exit menu' position 2)
            cur_state = VVC_STATE_SHOW_TIME;
            cursor_position = 0;
        }
        // Also reset cursor position to 0.
        cursor_position = 0;
    }
}

/*
 * Process the 'set time' state.
 */
void process_set_time_state() {
    // Draw centered 'SET TIME:'
    char set_time_buffer[10] = { 'S', 'E', 'T', ' ', 'T',
                                 'I', 'M', 'E', ':', '\0' };
    oled_draw_big_text(18, 26, set_time_buffer, 1);

    // Draw the currently-chosen time to the 7-segment displays.
    // Pull the current latch pin low.
    GPIOA->ODR &= ~IOA_595_LATCH_PIN;
    int cur_digit = 0xFF;
    // Blank the digits being set every other (seconds%2)
    // Minutes tens/ones.
    if (cursor_position == 1 && (time_word & 0x00000001)) {
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    else {
        cur_digit = cur_minutes % 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        cur_digit = cur_minutes / 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    // Hours tens/ones.
    if (cursor_position == 0 && (time_word & 0x00000001)) {
        cur_digit = 0xFF;
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    else {
        cur_digit = cur_hours % 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        cur_digit = cur_hours / 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    // Pull the latch pin high.
    GPIOA->ODR |= IOA_595_LATCH_PIN;

    // Check input.
    // Up/Down buttons move the current selection (hours or minutes).
    if (((~GPIOA->IDR) & IOA_BUTTON_UP) &&
        !(last_button_state & IOA_BUTTON_UP)) {
        if (cursor_position == 0) {
            ++cur_hours;
            if (cur_hours >= 24) { cur_hours = 0; }
        }
        else if (cursor_position == 1) {
            ++cur_minutes;
            if (cur_minutes >= 60) { cur_minutes = 0; }
        }
    }
    else if (((~GPIOA->IDR) & IOA_BUTTON_DOWN) &&
             !(last_button_state & IOA_BUTTON_DOWN)) {
        if (cursor_position == 0) {
            if (cur_hours == 0) { cur_hours = 23; }
            else { --cur_hours; }
        }
        else if (cursor_position == 1) {
            if (cur_minutes == 0) { cur_minutes = 59; }
            else { --cur_minutes; }
        }
    }
    // If Select button is pressed, progress one step. Use cursor position
    // Step 0: Set 'hours'
    // Step 1: Set 'minutes'
    // Step 2: Set time and back to default 'show time' screen.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        if (cursor_position == 0) {
            cursor_position = 1;
        }
        else {
            unsigned int hours_enc = cur_hours / 10;
            hours_enc = hours_enc << 4;
            hours_enc |= (cur_hours % 10);
            unsigned int mins_enc = cur_minutes / 10;
            mins_enc = mins_enc << 4;
            mins_enc |= (cur_minutes % 10);
            ds3231_set_time(I2C1_BASE, hours_enc, mins_enc);
            cur_state = VVC_STATE_SHOW_TIME;
            cursor_position = 0;
        }
    }
}

/*
 * Process the 'set alarm time' state.
 */
void process_set_alarm_state() {
    // Draw centered 'SET ALARM:'
    char set_alarm_buffer[11] = { 'S', 'E', 'T', ' ', 'A',
                                  'L', 'A', 'R', 'M', ':', '\0' };
    oled_draw_big_text(11, 26, set_alarm_buffer, 1);

    // Draw the current hours/minutes.
    // Draw the currently-chosen time to the 7-segment displays.
    // Pull the current latch pin low.
    GPIOA->ODR &= ~IOA_595_LATCH_PIN;
    int cur_digit = 0xFF;
    // Blank the digits being set every other (seconds%2)
    // Minutes tens/ones.
    if (cursor_position == 1 && (time_word & 0x00000001)) {
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    else {
        cur_digit = cur_minutes % 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        cur_digit = cur_minutes / 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    // Hours tens/ones.
    if (cursor_position == 0 && (time_word & 0x00000001)) {
        cur_digit = 0xFF;
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        shift_byte_out(cur_digit, &GPIOA->ODR,
                       IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    else {
        cur_digit = cur_hours % 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
        cur_digit = cur_hours / 10;
        shift_7_segment_out(cur_digit, &GPIOA->ODR,
                            IOA_595_CLOCK_PIN, IOA_595_DATA_PIN);
    }
    // Pull the latch pin high.
    GPIOA->ODR |= IOA_595_LATCH_PIN;

    // Check input.
    // Up/Down buttons move the current selection (hours or minutes).
    if (((~GPIOA->IDR) & IOA_BUTTON_UP) &&
        !(last_button_state & IOA_BUTTON_UP)) {
        if (cursor_position == 0) {
            ++cur_hours;
            if (cur_hours >= 24) { cur_hours = 0; }
        }
        else if (cursor_position == 1) {
            ++cur_minutes;
            if (cur_minutes >= 60) { cur_minutes = 0; }
        }
    }
    else if (((~GPIOA->IDR) & IOA_BUTTON_DOWN) &&
             !(last_button_state & IOA_BUTTON_DOWN)) {
        if (cursor_position == 0) {
            if (cur_hours == 0) { cur_hours = 23; }
            else { --cur_hours; }
        }
        else if (cursor_position == 1) {
            if (cur_minutes == 0) { cur_minutes = 59; }
            else { --cur_minutes; }
        }
    }
    // If Select button is pressed, progress one step. Use cursor position
    // Step 0: Set 'hours'
    // Step 1: Set 'minutes'
    // Step 2: Set time and back to default 'show time' screen.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        if (cursor_position == 0) {
            cursor_position = 1;
        }
        else {
            unsigned int hours_enc = cur_hours / 10;
            hours_enc = hours_enc << 4;
            hours_enc |= (cur_hours % 10);
            unsigned int mins_enc = cur_minutes / 10;
            mins_enc = mins_enc << 4;
            mins_enc |= (cur_minutes % 10);
            ds3231_set_alarm_1_time(I2C1_BASE, hours_enc, mins_enc);
            cur_state = VVC_STATE_SHOW_TIME;
            cursor_position = 0;
            alarm_word = ds3231_get_alarm_1(I2C1_BASE);
            alarm_word = alarm_word << 8;
        }
    }
}

/*
 * Process the 'set alarm days' state.
 */
void process_set_alarm_days_state() {
    // Draw centered 'ALARM DAYS:'
    char set_alarm_days_buffer[12] = { 'A', 'L', 'A', 'R', 'M', ' ',
                                       'D', 'A', 'Y', 'S', ':', '\0' };
    oled_draw_big_text(5, 26, set_alarm_days_buffer, 1);

    // Check input.
    // Up/Down buttons do nothing.
    // If Select button is pressed, switch to the 'show time' state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        cur_state = VVC_STATE_SHOW_TIME;
    }
}

/*
 * Process the 'set alarm tone' state.
 */
void process_set_alarm_tone_state() {
    // Draw centered 'ALARM TONE:'
    char set_alarm_days_buffer[12] = { 'A', 'L', 'A', 'R', 'M', ' ',
                                       'T', 'O', 'N', 'E', ':', '\0' };
    oled_draw_big_text(5, 16, set_alarm_days_buffer, 1);

    // Check input.
    // Up/Down buttons do nothing.
    // If Select button is pressed, switch to the 'show time' state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        cur_state = VVC_STATE_SHOW_TIME;
    }
}

/*
 * Process the 'set alarm on/off' menu state.
 */
void process_set_alarm_state_state() {
    // Draw centered 'ALARM ON?'
    char set_alarm_days_buffer[10] = { 'A', 'L', 'A', 'R', 'M',
                                      ' ', 'O', 'N', '?', '\0' };
    oled_draw_big_text(18, 16, set_alarm_days_buffer, 1);

    // Check input.
    // Up/Down buttons do nothing.
    // If Select button is pressed, switch to the 'show time' state.
    if (((~GPIOA->IDR) & IOA_BUTTON_SELECT) &&
        !(last_button_state & IOA_BUTTON_SELECT)) {
        cur_state = VVC_STATE_SHOW_TIME;
    }
}
