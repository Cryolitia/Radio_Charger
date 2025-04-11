#include "ina226.h"
#include <debug.h>

UINT16 _config = INA226_MODE_TRIG_SHUNT_AND_BUS | INA226_VSH_8244uS | INA226_VBUS_8244uS | INA226_AVG_4;

UINT16 read (UINT8 reg) {
    I2C_AcknowledgeConfig (I2C1, ENABLE);

    while (I2C_GetFlagStatus (I2C_PORT, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_GenerateSTART (I2C_PORT, ENABLE);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress (I2C_PORT, INA226_ADDRESS, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    I2C_SendData (I2C_PORT, reg);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C_GenerateSTART (I2C_PORT, ENABLE);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress (I2C_PORT, INA226_ADDRESS, I2C_Direction_Receiver);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;

    UINT8 buffer[2] = {0xCC, 0xCC};
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    if (I2C_GetFlagStatus (I2C_PORT, I2C_FLAG_RXNE) != RESET) {
        buffer[0] = I2C_ReceiveData (I2C_PORT);
    }
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    if (I2C_GetFlagStatus (I2C_PORT, I2C_FLAG_RXNE) != RESET) {
        buffer[1] = I2C_ReceiveData (I2C_PORT);
    }

    I2C_AcknowledgeConfig (I2C1, DISABLE);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    I2C_GenerateSTOP (I2C_PORT, ENABLE);

    return buffer[0] << 8 | buffer[1];
}

void write (UINT8 reg, UINT16 data) {
    I2C_AcknowledgeConfig (I2C1, ENABLE);

    while (I2C_GetFlagStatus (I2C_PORT, I2C_FLAG_BUSY) != RESET)
        ;

    I2C_GenerateSTART (I2C_PORT, ENABLE);

    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress (I2C_PORT, INA226_ADDRESS, I2C_Direction_Transmitter);

    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        ;
    I2C_SendData (I2C_PORT, reg);

    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        ;
    I2C_SendData (I2C_PORT, (data & 0xFF00) >> 8);
    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        ;
    I2C_SendData (I2C_PORT, data & 0x00FF);

    while (!I2C_CheckEvent (I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP (I2C_PORT, ENABLE);
}

void INA226_INIT (UINT16 config) {
    _config = config | (0b100 << 12);
    printf ("INA226 Init");
    write (INA226_CONFIG, _config);
    UINT16 manuf_id = read (INA226_MANUF_ID);
    printf ("MANUF_ID: 0x%04x\n", manuf_id);
    UINT16 die_id = read (INA226_DIE_ID);
    printf ("DIE_ID: 0x%04x\n", die_id);
    UINT16 config_id = read (INA226_CONFIG);
    printf ("Config: 0x%04x\n", config_id);
    write (INA226_CALIB, INA226_CALIB_VAL);
}

void INA226_START (void) {
    write (INA226_CONFIG, _config);
}

// return in V
float INA226_READ_VBUS (void) {
    UINT16 VBUS_REG = read (INA226_BUSV);
    return VBUS_REG * 0.00125f;
}

// return in mV
float INA226_READ_SHUNTV (void) {
    UINT16 SHUNTV_REG = read (INA226_SHUNTV);
    return SHUNTV_REG * 2.5 / 1000.0;
}

// return in mA
float INA226_READ_CURRENT (void) {
    UINT16 CURRENT_REG = read (INA226_CURRENT);
    return CURRENT_REG * INA226_CURRENTLSB;
}

// return in mW
float INA226_READ_POWER (void) {
    UINT16 POWER_REG = read (INA226_POWER);
    return POWER_REG * INA226_CURRENTLSB * 25.0F;
}