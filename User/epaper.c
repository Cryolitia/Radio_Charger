#include "epaper.h"

uint8_t tile_buf_height;
uint8_t *buf;
struct u8g2_helper_t u8g2_epaper = {
    {},
    250,
    122,
    EPAPER_PADDING_X,
    EPAPER_PADDING_Y,
    EPAPER_GUTTER_X,
};

static u8x8_display_info_t u8x8_jd79661_250x122_noname_display_info =
    {
        /* chip_enable_level = */ 0,
        /* chip_disable_level = */ 1,

        /* post_chip_enable_wait_ns = */ 0,
        /* pre_chip_disable_wait_ns = */ 0,
        /* reset_pulse_width_ms = */ 0,
        /* post_reset_wait_ms = */ 0,   /**/
        /* sda_setup_time_ns = */ 0,    /* */
        /* sck_pulse_width_ns = */ 0,   /*  */
        /* sck_clock_hz = */ 4000000UL, /* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
        /* spi_mode = */ 0,             /* active high, rising edge */
        /* i2c_bus_clock_100kHz = */ 4, /* 400KHz */
        /* data_setup_time_ns = */ 0,
        /* write_pulse_width_ns = */ 0,
        /* tile_width = */ 16,
        /* tile_height = */ 32,
        /* default_x_offset = */ 0,  /*  */
        /* flipmode_x_offset = */ 0, /*  */
        /* pixel_width = */ 122,
        /* pixel_height = */ 250};

/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    SPI_SSOutputCmd (SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init (SPI1, &SPI_InitStructure);

    SPI_Cmd (SPI1, ENABLE);
}

void Epaper_READBUSY() {
    while (1) {
        if (OLED_BUSY() == 0) {
            break;
        }
    }
}

// 模拟SPI时序
void OLED_WR_Bus (u8 dat) {
    SPI_Cmd (SPI1, ENABLE);
    while (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData (SPI1, dat);
    Delay_Us (8);
    SPI_Cmd (SPI1, DISABLE);
}

// 写入一个命令
void OLED_WR_REG (u8 reg) {
    OLED_DC_Clr();
    OLED_WR_Bus (reg);
    OLED_DC_Set();
}

// 写入一个字节
void OLED_WR_DATA8 (u8 dat) {
    OLED_WR_Bus (dat);
}

void EPD_Update (void) {
    OLED_WR_REG (0x22);  // Display Update Control
    OLED_WR_DATA8 (0xF7);
    OLED_WR_REG (0x20);  // Activate Display Update Sequence
    Epaper_READBUSY();
}

void EPD_PartUpdate (void) {
    OLED_WR_REG (0x22);
    OLED_WR_DATA8 (0xFF);
    OLED_WR_REG (0x20);
    Epaper_READBUSY();
}

// 初始化GPIO
void OLED_GPIOInit (void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_SetBits (GPIOA, GPIO_Pin_1 | GPIO_Pin_3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
}

// 刷新前准备
void Epaper_Start (void) {
    OLED_WR_REG (0x4E);  // set RAM x address count to 0;
    OLED_WR_DATA8 (0x00);
    OLED_WR_REG (0x4F);  // set RAM y address count to 0X199;
    OLED_WR_DATA8 (0x27);
    OLED_WR_DATA8 (0x01);

    Epaper_READBUSY();
}

void Epaper_ClearBuffer (struct u8g2_helper_t *u8g2) {
    size_t cnt;
    cnt = u8g2_GetU8x8 (u8g2)->display_info->tile_width;
    cnt *= u8g2->u8g2.tile_buf_height;
    cnt *= 8;
    memset (u8g2->u8g2.tile_buf_ptr, 0xFF, cnt);
}

// 更新到显存
void EPD_Display (unsigned char *Image) {
    unsigned int Width, Height, i, j;
    u32 k = 0;
    Width = 250;
    Height = 16;
    OLED_WR_REG (0x24);
    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++) {
            OLED_WR_DATA8 (Image[k]);
            k++;
        }
    }
}

void Epaper_Init() {
    OLED_GPIOInit();
    SPI_1Lines_HalfDuplex_Init();

    u8g2_SetupDisplay (&u8g2_epaper, u8x8_dummy_cb, u8x8_dummy_cb, u8x8_dummy_cb, u8x8_gpio_and_delay_ch32);
    u8x8_d_helper_display_setup_memory (&u8g2_epaper.u8g2.u8x8, &u8x8_jd79661_250x122_noname_display_info);
    buf = u8g2_m_16_32_f (&tile_buf_height);
    u8g2_SetupBuffer (&u8g2_epaper.u8g2, buf, tile_buf_height, u8g2_ll_hvline_horizontal_right_lsb, U8G2_R1);
    u8g2_SetClipWindow (&u8g2_epaper.u8g2, OLED_PADDING_X, OLED_PADDING_Y, u8g2_epaper.pixel_x - OLED_PADDING_X, u8g2_epaper.pixel_y - OLED_PADDING_Y);
    u8g2_SetFont (&u8g2_epaper.u8g2, U8G2_FONT);
    u8g2_ClearBuffer (&u8g2_epaper.u8g2);

    OLED_RES_Clr();  // Module reset
    Delay_Ms (20);   // At least 10ms delay
    OLED_RES_Set();
    Delay_Ms (20);   // At least 10ms delay
    Epaper_READBUSY();

    OLED_WR_REG (0x12);  // SWRESET
    Epaper_READBUSY();

    OLED_WR_REG (0x01);  // Driver output control
    OLED_WR_DATA8 (0x27);
    OLED_WR_DATA8 (0x01);
    OLED_WR_DATA8 (0x01);

    OLED_WR_REG (0x11);  // data entry mode
    OLED_WR_DATA8 (0x01);

    OLED_WR_REG (0x44);    // set Ram-X address start/end position
    OLED_WR_DATA8 (0x00);
    OLED_WR_DATA8 (0x0F);  // 0x0F-->(15+1)*8=128

    OLED_WR_REG (0x45);    // set Ram-Y address start/end position
    OLED_WR_DATA8 (0x27);  // 0xF9-->(249+1)=250
    OLED_WR_DATA8 (0x01);
    OLED_WR_DATA8 (0x00);
    OLED_WR_DATA8 (0x00);

    OLED_WR_REG (0x3C);  // BorderWavefrom
    OLED_WR_DATA8 (0x05);

    OLED_WR_REG (0x21);  //  Display update control
    OLED_WR_DATA8 (0x00);
    OLED_WR_DATA8 (0x80);

    OLED_WR_REG (0x18);  // Read built-in temperature sensor
    OLED_WR_DATA8 (0x80);

    Epaper_Start();

    Epaper_ClearBuffer (&u8g2_epaper);

    u8g2_SetDrawColor (&u8g2_epaper.u8g2, 0);
    u8g2_SetFontPosCenter (&u8g2_epaper.u8g2);
    u8g2_draw_center_x2 (&u8g2_epaper, WELECOME_STR);
    u8g2_SetFontPosBaseline (&u8g2_epaper.u8g2);

    EPD_Display (buf);
    EPD_Update();
}

void epaper_draw_main_page() {
    Epaper_Start();

    char buffer[20] = {0};
    Epaper_ClearBuffer (&u8g2_epaper);

    if (!drv_prv.pd_status) {
        u8g2_draw_left (&u8g2_epaper, "USB PD 未连接", 1, 3, 0, 0);
    } else {
        u8g2_draw_left (&u8g2_epaper, "PD 已连接", 3, 3, 0, 0);

        sprintf (buffer, "%d V", drv_prv.pd_voltage / 1000);
        u8g2_draw_right (&u8g2_epaper, buffer, 3, 3, 1, 0);

        sprintf (buffer, "%d A", drv_prv.pd_current / 1000);
        u8g2_draw_right (&u8g2_epaper, buffer, 3, 3, 2, 0);
    }

    u8g2_draw_left (&u8g2_epaper, "电压", 2, 3, 0, 1);
    sprintf (buffer, "%d.%02d V", (int)drv_prv.vbus, (int)(drv_prv.vbus * 100) % 100);
    u8g2_draw_right (&u8g2_epaper, buffer, 2, 3, 0, 1);

    u8g2_draw_left (&u8g2_epaper, "电流", 2, 3, 1, 1);
    sprintf (buffer, "%d mA", (int)(drv_prv.current + 0.5));
    u8g2_draw_right (&u8g2_epaper, buffer, 2, 3, 1, 1);

    u8g2_draw_left (&u8g2_epaper, "温度", 2, 3, 0, 2);
    sprintf (buffer, "%d.%d °C", (int)drv_prv.temperature, (int)(drv_prv.temperature * 10) % 10);
    u8g2_draw_right (&u8g2_epaper, buffer, 2, 3, 0, 2);

    u8g2_draw_left (&u8g2_epaper, "功率", 2, 3, 1, 2);
    sprintf (buffer, "%d mW", (int)(drv_prv.power + 0.5));
    u8g2_draw_right (&u8g2_epaper, buffer, 2, 3, 1, 2);

    EPD_Display (buf);
}