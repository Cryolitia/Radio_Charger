#ifndef EPAPER_HEADER
#define EPAPER_HEADER

#include "debug.h"
#include "main.h"
#include "u8g2.h"

#define OLED_RES_Clr() GPIO_ResetBits (GPIOA, GPIO_Pin_1)  // RES
#define OLED_RES_Set() GPIO_SetBits (GPIOA, GPIO_Pin_1)

#define OLED_DC_Clr() GPIO_ResetBits (GPIOA, GPIO_Pin_3)  // DC
#define OLED_DC_Set() GPIO_SetBits (GPIOA, GPIO_Pin_3)

#define OLED_BUSY() GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_2)

void Epaper_Init();
void epaper_draw_main_page();
void EPD_Update (void);
void EPD_PartUpdate (void);

uint8_t u8x8_gpio_and_delay_ch32 (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif