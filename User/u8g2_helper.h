#ifndef U8G2_HELPER_HEADER
#define U8G2_HELPER_HEADER

#include "debug.h"
#include "main.h"

uint8_t u8x8_gpio_and_delay_ch32 (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

struct u8g2_helper_t {
    u8g2_t u8g2;
    int pixel_x;
    int pixel_y;
    int padding_x;
    int padding_y;
    int gutter_x;
    int moving_y;
};

void u8g2_draw_center (struct u8g2_helper_t *u8g2_helper, const char *str);
void u8g2_draw_left (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);
void u8g2_draw_right (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);
void u8g2_draw_center_c (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);

void u8g2_draw_center_x2 (struct u8g2_helper_t *u8g2_helper, const char *str);
void u8g2_draw_left_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);
void u8g2_draw_right_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);
void u8g2_draw_center_c_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column);

#endif