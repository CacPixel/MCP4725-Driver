#ifndef __MCP4725_H__
#define __MCP4725_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"
#include "stdbool.h"

#define MCP4725_TIMEOUT 5
#define MCP4725_VDD 5000.0
#define MCP4725_ADDR 0x00c0
#define MCP4725_GENERALCALL_ADDR 0x0000

    typedef struct {
        bool POR;
        uint16_t dacRegVal;
        uint16_t dacEEPROMVal;
        uint8_t PD_Reg;
        uint8_t PD_EEPROM;
        bool isOk;
    } MCP4725_RDDataTypeDef;

    typedef enum {
        MCP_PM_NORMAL = 0x00U,
        MCP_PM_1K,
        MCP_PM_100K,
        MCP_PM_500K
    } MCP4725_PowerModeTypeDef;

    HAL_StatusTypeDef MCP4725_SetVoltage(I2C_HandleTypeDef hi2c, uint16_t Voltage);
    HAL_StatusTypeDef MCP4725_SetDACVal(I2C_HandleTypeDef hi2c, uint16_t dacVal);
    HAL_StatusTypeDef MCP4725_WriteDACRegFastMode(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat);
    HAL_StatusTypeDef MCP4725_WriteDACReg(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat);
    HAL_StatusTypeDef MCP4725_WriteAll(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat, MCP4725_PowerModeTypeDef mode);
    HAL_StatusTypeDef MCP4725_Read(I2C_HandleTypeDef hi2c, bool shouldRepeat, MCP4725_RDDataTypeDef* MCP4725RdData);
    int16_t MCP4725_ReadVoltage(I2C_HandleTypeDef hi2c);
    uint16_t MCP4725_ReadDACVal(I2C_HandleTypeDef hi2c);
    HAL_StatusTypeDef MCP4725_CheckEEPROM(I2C_HandleTypeDef hi2c, bool shouldRepeat);
    HAL_StatusTypeDef MCP4725_PowerControlWithDACValue(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode, uint16_t dacVal);
    HAL_StatusTypeDef MCP4725_PowerControlWithVoltage(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode, int16_t voltage);
    HAL_StatusTypeDef MCP4725_PowerControl(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode);
    uint16_t MCP4725_getDACVal(int16_t voltage);
    int16_t MCP4725_getVoltage(uint16_t dacVal);
    HAL_StatusTypeDef MCP4725_Reset(I2C_HandleTypeDef hi2c);
    HAL_StatusTypeDef MCP4725_PowerDown(I2C_HandleTypeDef hi2c);
    HAL_StatusTypeDef MCP4725_WakeUp(I2C_HandleTypeDef hi2c);
    HAL_StatusTypeDef MCP4725_CheckI2CSpeed(I2C_HandleTypeDef hi2c);

#ifdef __cplusplus
}
#endif

#endif
