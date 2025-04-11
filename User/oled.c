#include <debug.h>
#include "u8g2.h"
#include "main.h"
#include "oled.h"

#ifdef I2C_OLED_ENABLE
struct u8g2_helper_t u8g2_oled = {
    {},
    128,
    64,
    OLED_PADDING_X,
    OLED_PADDING_Y,
    OLED_GUTTER_X,
};
#endif

void DMA_Tx_Init (DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize) {
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit (DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA_CHx, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    DMA_ITConfig (DMA1_Channel6, DMA_IT_TC, ENABLE);
}

void I2C_Master_Transmit (I2C_TypeDef *I2Cx, uint16_t slave_add, uint8_t *buff, uint8_t idx) {
    I2C_AcknowledgeConfig (I2Cx, ENABLE);

    DMA_Tx_Init (DMA1_Channel6, (u32)&I2C1->DATAR, (u32)buff, idx);

    while (I2C_GetFlagStatus (I2Cx, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_GenerateSTART (I2Cx, ENABLE);

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress (I2Cx, slave_add, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    DMA_Cmd (DMA1_Channel6, ENABLE);

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP (I2Cx, ENABLE);
}

uint8_t u8x8_byte_ch32_hw_i2c (u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    // u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER
    // add extra byte for the i2c address
    static uint8_t buffer[34];
    static uint8_t buf_idx;
    uint8_t *data;
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0) {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;
    case U8X8_MSG_BYTE_INIT:
        // add your custom code to init i2c subsystem
        break;
    case U8X8_MSG_BYTE_SET_DC:
        //	 ignored for i2c
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        I2C_Master_Transmit (I2C1, OLED_ADDRESS, (uint8_t *)buffer, buf_idx);
        break;
    default:
        return 0;
    }
    return 1;
}

void DMA1_Channel6_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      DMA1_Channel6_IRQHandler
 *
 * @brief   This function handles DMA1 channel6 exception.
 *
 * @return  none
 */
void DMA1_Channel6_IRQHandler() {
    if (DMA_GetITStatus (DMA1_IT_TC6) != RESET) {
        DMA_Cmd (DMA1_Channel6, DISABLE);
        DMA_ClearITPendingBit (DMA1_IT_TC6);
    }
}

void i2c_oled_init() {
    u8g2_Setup_ssd1306_i2c_128x64_noname_f (&u8g2_oled.u8g2, U8G2_R0, u8x8_byte_ch32_hw_i2c, u8x8_gpio_and_delay_ch32);
    u8g2_InitDisplay (&u8g2_oled.u8g2);  // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave (&u8g2_oled.u8g2, 0);
    u8g2_SetClipWindow (&u8g2_oled.u8g2, OLED_PADDING_X, OLED_PADDING_Y, u8g2_oled.pixel_x - OLED_PADDING_X, u8g2_oled.pixel_y - OLED_PADDING_Y);
    u8g2_SetFont (&u8g2_oled.u8g2, U8G2_FONT);

    drv_prv.font_height = u8g2_GetAscent (&u8g2_oled.u8g2);

    u8g2_ClearBuffer (&u8g2_oled.u8g2);
    u8g2_draw_center (&u8g2_oled, WELECOME_STR);
    u8g2_SendBuffer (&u8g2_oled.u8g2);
}

void i2c_oled_draw_main_page() {
    char buffer[20] = {0};
    u8g2_ClearBuffer (&u8g2_oled.u8g2);

    if (!drv_prv.pd_status) {
        u8g2_draw_left (&u8g2_oled, "USB PD N/A", 1, 3, 0, 0);
    } else {
        u8g2_draw_left (&u8g2_oled, "PD", 3, 3, 0, 0);

        sprintf (buffer, "%d V", drv_prv.pd_voltage / 1000);
        u8g2_draw_right (&u8g2_oled, buffer, 3, 3, 1, 0);

        sprintf (buffer, "%d A", drv_prv.pd_current / 1000);
        u8g2_draw_right (&u8g2_oled, buffer, 3, 3, 2, 0);
    }

    // u8g2_draw_left(&u8g2_oled, "电压", 2, 3, 0, 1);
    sprintf (buffer, "%d.%02d V", (int)drv_prv.vbus, (int)(drv_prv.vbus * 100) % 100);
    u8g2_draw_right (&u8g2_oled, buffer, 2, 3, 0, 1);

    // u8g2_draw_left(&u8g2_oled, "电流", 2, 3, 1, 1);
    sprintf (buffer, "%d mA", (int)(drv_prv.current + 0.5));
    u8g2_draw_right (&u8g2_oled, buffer, 2, 3, 1, 1);

    // u8g2_draw_left(&u8g2_oled, "温度", 2, 3, 0, 2);
    sprintf (buffer, "%d.%d °C", (int)drv_prv.temperature, (int)(drv_prv.temperature * 10) % 10);
    u8g2_draw_right (&u8g2_oled, buffer, 2, 3, 0, 2);

    // u8g2_draw_left(&u8g2_oled, "功率", 2, 3, 1, 2);
    sprintf (buffer, "%d mW", (int)(drv_prv.power + 0.5));
    u8g2_draw_right (&u8g2_oled, buffer, 2, 3, 1, 2);

    u8g2_SendBuffer (&u8g2_oled.u8g2);
}