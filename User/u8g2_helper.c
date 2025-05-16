#include "u8g2_helper.h"

/*********************************************************************
 * @fn      Delay_100_Ns
 *
 * @brief   100 Nanosecond Delay Time.
 *
 * @param   n - 100 Nanosecond number.
 *
 * @return  None
 */
void Delay_100_Ns (uint32_t n) {
    uint32_t i;

    SysTick->SR &= ~(1 << 0);
    i = (uint32_t)n * SystemCoreClock / 10000000 + 1;

    SysTick->CMP = i;
    SysTick->CTLR |= (1 << 4);
    SysTick->CTLR |= (1 << 5) | (1 << 0);

    while ((SysTick->SR & (1 << 0)) != (1 << 0))
        ;
    SysTick->CTLR &= ~(1 << 0);
}

uint8_t u8x8_gpio_and_delay_ch32 (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:  // called once during init phase of u8g2/u8x8
        break;                          // can be used to setup pins
    case U8X8_MSG_DELAY_100NANO:        // delay arg_int * 100 nano seconds
        Delay_100_Ns (arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:  // delay arg_int * 10 micro seconds
        Delay_Us (arg_int * 10);
        break;
    case U8X8_MSG_DELAY_MILLI:  // delay arg_int * 1 milli second
        Delay_Ms (arg_int);
        break;
    default:
        return 0;
    }

    return 1;
}

void u8g2_draw_center (struct u8g2_helper_t *u8g2_helper, const char *str) {
    u8g2_SetFontPosCenter (&u8g2_helper->u8g2);
    int width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, WELECOME_STR);
    u8g2_DrawUTF8 (&u8g2_helper->u8g2, (u8g2_helper->pixel_x - width) / 2, u8g2_helper->pixel_y / 2, WELECOME_STR);
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
}

void u8g2_draw_center_c (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    int utf8_width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, str);
    float x_width = (u8g2_helper->pixel_x - 2 * u8g2_helper->padding_x - (row - 1) * u8g2_helper->gutter_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height * 2) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * pos_row + pos_row * u8g2_helper->gutter_x - (x_width - utf8_width) / 2;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * 2 * (pos_column + 1) + gutter_y * pos_column + drv_prv.font_height * 2 / 2.0;

    u8g2_SetFontPosCenter (&u8g2_helper->u8g2);
    u8g2_DrawUTF8 (&u8g2_helper->u8g2, start_x, start_y, str);
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
}

void u8g2_draw_left (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
    float x_width = (u8g2_helper->pixel_x - 2 * u8g2_helper->padding_x - (row - 1) * u8g2_helper->gutter_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * pos_row + pos_row * u8g2_helper->gutter_x;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * (pos_column + 1) + gutter_y * pos_column + u8g2_helper->moving_y;

    u8g2_DrawUTF8 (&u8g2_helper->u8g2, start_x, start_y, str);
}

void u8g2_draw_right (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
    int utf8_width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, str);
    float x_width = (u8g2_helper->pixel_x - 2.0 * u8g2_helper->padding_x - (row - 1) * u8g2_helper->gutter_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * (pos_row + 1) + pos_row * u8g2_helper->gutter_x - utf8_width;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * (pos_column + 1) + gutter_y * pos_column + u8g2_helper->moving_y;

    u8g2_DrawUTF8 (&u8g2_helper->u8g2, start_x, start_y, str);
}

void u8g2_draw_center_x2 (struct u8g2_helper_t *u8g2_helper, const char *str) {
    u8g2_SetFontPosCenter (&u8g2_helper->u8g2);
    int width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, WELECOME_STR) * 2;
    u8g2_DrawUTF8X2 (&u8g2_helper->u8g2, (u8g2_helper->pixel_x - width) / 2, u8g2_helper->pixel_y / 2, WELECOME_STR);
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
}

void u8g2_draw_center_c_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    int utf8_width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, str) * 2;
    float x_width = (u8g2_helper->pixel_x - 2 * u8g2_helper->padding_x - (row - 1) * u8g2_helper->gutter_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height * 2) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * pos_row + pos_row * u8g2_helper->gutter_x - (x_width - utf8_width) / 2;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * 2 * (pos_column + 1) + gutter_y * pos_column + drv_prv.font_height * 2 / 2.0;

    u8g2_SetFontPosCenter (&u8g2_helper->u8g2);
    u8g2_DrawUTF8X2 (&u8g2_helper->u8g2, start_x, start_y, str);
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
}

void u8g2_draw_left_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
    float x_width = (u8g2_helper->pixel_x - 2 * u8g2_helper->padding_x - (row - 1) * u8g2_helper->gutter_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height * 2) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * pos_row + pos_row * u8g2_helper->gutter_x;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * 2 * (pos_column + 1) + gutter_y * pos_column;

    u8g2_DrawUTF8X2 (&u8g2_helper->u8g2, start_x, start_y, str);
}

void u8g2_draw_right_x2 (struct u8g2_helper_t *u8g2_helper, const char *str, int row, int column, int pos_row, int pos_column) {
    u8g2_SetFontPosBaseline (&u8g2_helper->u8g2);
    int utf8_width = u8g2_GetUTF8Width (&u8g2_helper->u8g2, str - (row - 1) * u8g2_helper->gutter_x) * 2;
    float x_width = (u8g2_helper->pixel_x - 2.0 * u8g2_helper->padding_x) / ((float)row);
    float gutter_y = (u8g2_helper->pixel_y - 2.0 * u8g2_helper->padding_y - column * drv_prv.font_height * 2) / ((float)(column - 1));
    float start_x = u8g2_helper->padding_x + x_width * (pos_row + 1) + pos_row * u8g2_helper->gutter_x - utf8_width;
    float start_y = u8g2_helper->padding_y + drv_prv.font_height * 2 * (pos_column + 1) + gutter_y * pos_column;

    u8g2_DrawUTF8X2 (&u8g2_helper->u8g2, start_x, start_y, str);
}