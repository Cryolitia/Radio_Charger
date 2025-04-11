#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <debug.h>
#include <string.h>
#include "PD_Process.h"
#include "ds18b20.h"
#include "ina226.h"
#include "u8g2.h"
#include "oled.h"
#include "epaper.h"
#include "u8g2_helper.h"
#include "unifont.h"

#define DS18B20_ENABLE
// #define I2C_OLED_ENABLE
#define EPAPER_ENABLE
#define INA226_ENABLE

#define MAIN_THREAD_INTERVAL 1000

#define OLED_PADDING_X 5
#define OLED_PADDING_Y 5
#define OLED_GUTTER_X 0

#define EPAPER_FULL_REFRESH_INTERVAL 10
#define EPAPER_PADDING_X 20
#define EPAPER_PADDING_Y 20
#define EPAPER_GUTTER_X 20

#define U8G2_FONT u8g2_font_unifont

struct driver_private_data {
    int font_height;

    int pd_status;
    int pd_voltage;
    int pd_current;

    float vbus;
    int current;
    int power;

    float temperature;
};

extern struct driver_private_data drv_prv;

void TIM1_UP_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));
void TIM3_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

#define WELECOME_STR "Hello World!"

/************************** DS18B20 连接引脚定义********************************/
#define DS18B20_DQ_GPIO_CLK RCC_APB2Periph_GPIOA

#define DS18B20_DQ_GPIO_PORT GPIOA
#define DS18B20_DQ_GPIO_PIN GPIO_Pin_16

#define I2C_PORT I2C1
#define INA226_ADDRESS 0x40 << 1 /*INA226的地址*/

#define INA226_CALIB_VAL 0x0800
#define INA226_CURRENTLSB 0.25F  // mA/bit

#define OLED_ADDRESS 0x78

#endif  // MAIN_HEADER