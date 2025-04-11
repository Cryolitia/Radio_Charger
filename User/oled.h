#ifndef OLED_HEADER
#define OLED_HEADER

#include <debug.h>
#include "main.h"
#include "u8g2.h"

extern struct u8g2_helper_t u8g2_oled;

void i2c_oled_init();
void i2c_oled_draw_main_page();

#endif