/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/03
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "main.h"

volatile UINT8 Tim_Ms_Cnt = 0x00;
volatile UINT32 Tim_Second_Cnt = MAIN_THREAD_INTERVAL - 1000;

/*********************************************************************
 * @fn      TIM1_Init
 *
 * @brief   Initialize TIM1
 *
 * @return  none
 */
// 全局时间片计时器
void TIM1_Init (u16 arr, u16 psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit (TIM1, &TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit (TIM1, TIM_IT_Update);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    TIM_ITConfig (TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd (TIM1, ENABLE);
}

/*********************************************************************
 * @fn      TIM3_Init
 *
 * @brief   Initialize TIM3
 *
 * @return  none
 */
// 延迟读取计时器
void TIM3_Init (u16 arr, u16 psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit (TIM3, &TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit (TIM3, TIM_IT_Update);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    TIM_ITConfig (TIM3, TIM_IT_Update, ENABLE);
}

void IIC_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2c_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    I2c_InitStructure.I2C_ClockSpeed = 80000;
    I2c_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2c_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2c_InitStructure.I2C_OwnAddress1 = 0x02;
    I2c_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2c_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init (I2C1, &I2c_InitStructure);
    I2C_StretchClockCmd (I2C1, ENABLE);

    I2C_DMACmd (I2C1, ENABLE);

    I2C_Cmd (I2C1, ENABLE);
    I2C_AcknowledgeConfig (I2C1, ENABLE);
}

struct driver_private_data drv_prv;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (921600);
    memset (&drv_prv, 0, sizeof (struct driver_private_data));

    printf ("SystemClk:%d\n", SystemCoreClock);
    printf ("ChipID:%08x\n", DBGMCU_GetCHIPID());
    printf ("PD SNK TEST\n");

    PD_Init();

#ifdef DS18B20_ENABLE
    DS18B20_Init();
#endif
    IIC_Init();
#ifdef INA226_ENABLE
    INA226_INIT (INA226_MODE_TRIG_SHUNT_AND_BUS | INA226_VSH_8244uS | INA226_VBUS_8244uS | INA226_AVG_4);
#endif
#ifdef I2C_OLED_ENABLE
    i2c_oled_init();
#endif
#ifdef EPAPER_ENABLE
    Epaper_Init();
#endif

    ADC_Function_Init();
    ADC_SoftwareStartConvCmd (ADC1, ENABLE);

    TIM1_Init (999, 48 - 1);
    TIM3_Init (99, 12000 - 1);

    // DC-DC Enable
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    // GPIO_ResetBits (GPIOA, GPIO_Pin_17);
    GPIO_SetBits (GPIOA, GPIO_Pin_17);

    while (1) {
        /* Get the calculated timing interval value */
        TIM_ITConfig (TIM1, TIM_IT_Update, DISABLE);
        Tmr_Ms_Dlt = Tim_Ms_Cnt - Tmr_Ms_Cnt_Last;
        Tmr_Ms_Cnt_Last = Tim_Ms_Cnt;
        TIM_ITConfig (TIM1, TIM_IT_Update, ENABLE);
        PD_Ctl.Det_Timer += Tmr_Ms_Dlt;
        if (PD_Ctl.Det_Timer > 4) {
            PD_Ctl.Det_Timer = 0;
            PD_Det_Proc();
        }
        PD_Main_Proc();

        if (Tim_Second_Cnt >= MAIN_THREAD_INTERVAL) {
            TIM_ITConfig (TIM1, TIM_IT_Update, DISABLE);
            Tim_Second_Cnt = 0;
            TIM_ITConfig (TIM1, TIM_IT_Update, ENABLE);

#ifdef DS18B20_ENABLE
            // DS18B20
            DS18B20_Start_Convert_SkipRom();
#endif

#ifdef INA226_ENABLE
            // INA226
            INA226_START();
#endif
            ADC_SoftwareStartConvCmd (ADC1, ENABLE);

            // Delay 100ms
            TIM_Cmd (TIM3, ENABLE);
        }
    }
}

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 interrupt.
 *
 * @return  none
 */
void TIM1_UP_IRQHandler (void) {
    if (TIM_GetITStatus (TIM1, TIM_IT_Update) != RESET) {
        Tim_Ms_Cnt++;
        Tim_Second_Cnt++;
        TIM_ClearITPendingBit (TIM1, TIM_IT_Update);
    }
}

/*********************************************************************
 * @fn      TIM3_UP_IRQHandler
 *
 * @brief   This function handles TIM3 interrupt.
 *
 * @return  none
 */
void TIM3_IRQHandler (void) {
    static int refresh_count = 0;

    if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
        TIM_Cmd (TIM3, DISABLE);
        TIM_ClearITPendingBit (TIM3, TIM_IT_Update);

#ifdef DS18B20_ENABLE
        drv_prv.temperature = DS18B20_GetTemp_SkipRom();
#endif

#ifdef INA226_ENABLE
        drv_prv.vbus = INA226_READ_VBUS();
        drv_prv.current = INA226_READ_CURRENT();
        drv_prv.power = INA226_READ_POWER();
#endif

        printf ("Temp: %d.%d C\n", (int)drv_prv.temperature, (int)(drv_prv.temperature * 10) % 10);
        printf ("VBUS: %d.%03d V\n", (int)drv_prv.vbus, (int)(drv_prv.vbus * 1000) % 1000);
        printf ("Current: %d mA\n", (int)(drv_prv.current + 0.5));
        printf ("Power: %d mW\n", (int)(drv_prv.power + 0.5));
        if (drv_prv.vbus < 4.5) {
            drv_prv.pd_status = 0;
        }

        drv_prv.vout_voltage = get_output_voltage();
        printf ("output voltage: %d.%02d\n", (int)drv_prv.vout_voltage, (int)(drv_prv.vout_voltage * 100) % 100);

#ifdef I2C_OLED_ENABLE
        i2c_oled_draw_main_page();
#endif

#ifdef EPAPER_ENABLE
        epaper_draw_main_page();

        if (refresh_count == 0) {
            EPD_Update();
        } else {
            EPD_PartUpdate();
        }

        refresh_count++;

        if (refresh_count == EPAPER_FULL_REFRESH_INTERVAL) {
            refresh_count = 0;
        }
#endif
    }
}