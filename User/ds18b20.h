#ifndef _BSP_DS18B20_H
#define _BSP_DS18B20_H

#include <debug.h>
#include "main.h"

/************************** DS18B20 函数宏定义********************************/
#define DS18B20_DQ_0 GPIO_ResetBits (DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_1 GPIO_SetBits (DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_IN() GPIO_ReadInputDataBit (DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)

/************************** DS18B20 函数声明 ********************************/
uint8_t DS18B20_Init (void);
void DS18B20_ReadId (uint8_t *ds18b20_id);
void DS18B20_Start_Convert_SkipRom (void);
float DS18B20_GetTemp_SkipRom (void);
float DS18B20_GetTemp_MatchRom (uint8_t *ds18b20_id);

#endif /* __DS18B20_H */
