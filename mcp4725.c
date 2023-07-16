//Code by CacPixel
//Tested on STM32F103C8T6 with HAL Driver

#include "MCP4725.h"
#include "i2c.h"
#include "main.h"


HAL_StatusTypeDef MCP4725_SetVoltage(I2C_HandleTypeDef hi2c, uint16_t voltage) {
	uint16_t dacVal;
	dacVal = MCP4725_getDACVal(voltage);
	return MCP4725_WriteDACRegFastMode(hi2c, dacVal, false);
}

HAL_StatusTypeDef MCP4725_SetDACVal(I2C_HandleTypeDef hi2c, uint16_t dacVal) {
	return MCP4725_WriteDACRegFastMode(hi2c, dacVal, false);
}

HAL_StatusTypeDef MCP4725_WriteDACRegFastMode(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat) {
	if (MCP4725_CheckI2CSpeed(hi2c) != HAL_OK) return HAL_ERROR;
	uint8_t data[4] = { 0 };
	uint8_t size = shouldRepeat ? 4 : 2;
	data[0] = (0x0f00 & dacVal) >> 8;
	data[1] = (0x00ff & dacVal);
	if (shouldRepeat) {
		data[2] = data[0];
		data[3] = data[1];
	}
	return HAL_I2C_Master_Transmit(&hi2c, MCP4725_ADDR, data, size, MCP4725_TIMEOUT);
}

HAL_StatusTypeDef MCP4725_WriteDACReg(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat) {
	if (MCP4725_CheckI2CSpeed(hi2c) != HAL_OK) return HAL_ERROR;
	uint8_t data[6] = { 0 };
	uint8_t size = shouldRepeat ? 6 : 3;
	data[0] = 0b01000000;					//2nd byte
	data[1] = (0x0ff0 & dacVal) >> 4;		//3rd byte
	data[2] = (0x000f & dacVal) << 4;		//4th byte
	if (shouldRepeat) {
		data[3] = data[0];					//2nd byte
		data[4] = data[1];					//3rd byte
		data[5] = data[2];					//4th byte
	}
	return HAL_I2C_Master_Transmit(&hi2c, MCP4725_ADDR, data, size, MCP4725_TIMEOUT);
}

HAL_StatusTypeDef MCP4725_WriteAll(I2C_HandleTypeDef hi2c, uint16_t dacVal, bool shouldRepeat, MCP4725_PowerModeTypeDef mode) {
	if (MCP4725_CheckI2CSpeed(hi2c) != HAL_OK) return HAL_ERROR;
	if (MCP4725_CheckEEPROM(hi2c, shouldRepeat) == HAL_OK) {
		uint8_t data[6] = { 0 };
		uint8_t size = shouldRepeat ? 6 : 3;
		data[0] = 0b01100000;					//2nd byte
		switch (mode) {
		case MCP_PM_NORMAL:
			data[0] |= 0b00000000;
			break;
		case MCP_PM_1K:
			data[0] |= 0b00000010;
			break;
		case MCP_PM_100K:
			data[0] |= 0b00000100;
			break;
		case MCP_PM_500K:
			data[0] |= 0b00000110;
			break;
		default:
			return HAL_ERROR;
			break;
		}
		data[1] = (0x0ff0 & dacVal) >> 4;		//3rd byte
		data[2] = (0x000f & dacVal) << 4;		//4th byte
		if (shouldRepeat) {
			data[3] = data[0];					//2nd byte
			data[4] = data[1];					//3rd byte
			data[5] = data[2];					//4th byte
		}
		return HAL_I2C_Master_Transmit(&hi2c, MCP4725_ADDR, data, size, MCP4725_TIMEOUT);
	} else {
		return HAL_BUSY;
	}
}

HAL_StatusTypeDef MCP4725_Read(I2C_HandleTypeDef hi2c, bool shouldRepeat, MCP4725_RDDataTypeDef* MCP4725RdData) {
	if (MCP4725_CheckI2CSpeed(hi2c) != HAL_OK) return HAL_ERROR;
	uint8_t data[10] = { 0 };
	uint8_t size = shouldRepeat ? 10 : 5;
	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Receive(&hi2c, MCP4725_ADDR, data, size, MCP4725_TIMEOUT);
	if (status != HAL_OK) {
		return status;
	}
	MCP4725RdData->isOk = ((data[0] & 0b10000000) >> 7);
	MCP4725RdData->POR = ((data[0] & 0b01000000) >> 6);
	MCP4725RdData->PD_Reg = ((data[0] & 0b00000110) >> 1);
	MCP4725RdData->PD_EEPROM = ((data[3] & 0b01100000) >> 5);
	MCP4725RdData->dacRegVal = ((uint16_t)data[1] << 4) + (data[2] >> 4);
	MCP4725RdData->dacEEPROMVal = (((uint16_t)data[3] & 0x000f) << 8) + (data[4]);
	if (shouldRepeat) {
		status = MCP4725RdData->isOk == ((data[5] & 0b10000000) >> 7) ? HAL_OK : HAL_ERROR;
		status = MCP4725RdData->POR == ((data[5] & 0b01000000) >> 6) ? HAL_OK : HAL_ERROR;
		status = MCP4725RdData->PD_Reg == ((data[5] & 0b00000110) >> 1) ? HAL_OK : HAL_ERROR;
		status = MCP4725RdData->PD_EEPROM == ((data[8] & 0b01100000) >> 5) ? HAL_OK : HAL_ERROR;
		status = MCP4725RdData->dacRegVal == ((uint16_t)data[6] << 4) + (data[7] >> 4) ? HAL_OK : HAL_ERROR;
		status = MCP4725RdData->dacEEPROMVal == (((uint16_t)data[8] & 0x000f) << 8) + (data[9]) ? HAL_OK : HAL_ERROR;
	}
	return status;
}

int16_t MCP4725_ReadVoltage(I2C_HandleTypeDef hi2c) {
	MCP4725_RDDataTypeDef MCP4725RdData = { 0 };
	for (uint8_t i = 0; i < 5; i++) {
		if (MCP4725_Read(hi2c, false, &MCP4725RdData) == HAL_OK) {
			return MCP4725_getVoltage(MCP4725RdData.dacRegVal);
		}
	}
	return -1;
}

uint16_t MCP4725_ReadDACVal(I2C_HandleTypeDef hi2c) {
	MCP4725_RDDataTypeDef MCP4725RdData = { 0 };
	for (uint8_t i = 0; i < 5; i++) {
		if (MCP4725_Read(hi2c, false, &MCP4725RdData) == HAL_OK) {
			return MCP4725RdData.dacRegVal;
		}
	}
	return -1;
}

HAL_StatusTypeDef MCP4725_CheckEEPROM(I2C_HandleTypeDef hi2c, bool shouldRepeat) {
	MCP4725_RDDataTypeDef data = { 0 };
	HAL_StatusTypeDef status;
	status = MCP4725_Read(hi2c, shouldRepeat, &data);
	status = data.isOk ? status : HAL_BUSY;
	return status;
}

HAL_StatusTypeDef MCP4725_PowerControlWithDACValue(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode, uint16_t dacVal) {
	uint8_t data[4] = { 0 };
	uint8_t size = 2;
	data[0] = (0x0f00 & dacVal) >> 8;
	switch (mode) {
	case MCP_PM_NORMAL:
		data[0] |= 0b00000000;
		break;
	case MCP_PM_1K:
		data[0] |= 0b00010000;
		break;
	case MCP_PM_100K:
		data[0] |= 0b00100000;
		break;
	case MCP_PM_500K:
		data[0] |= 0b00110000;
		break;
	default:
		return HAL_ERROR;
		break;
	}
	data[1] = (0x00ff & dacVal);
	return HAL_I2C_Master_Transmit(&hi2c, MCP4725_ADDR, data, size, MCP4725_TIMEOUT);
}

HAL_StatusTypeDef MCP4725_PowerControlWithVoltage(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode, int16_t voltage) {
	uint16_t dacVal = MCP4725_getDACVal(voltage);
	return MCP4725_PowerControlWithDACValue(hi2c, mode, dacVal);
}

HAL_StatusTypeDef MCP4725_PowerControl(I2C_HandleTypeDef hi2c, MCP4725_PowerModeTypeDef mode) {
	MCP4725_RDDataTypeDef dacData = { 0 };
	HAL_StatusTypeDef readStatus = MCP4725_Read(hi2c, false, &dacData);
	if (readStatus != HAL_OK) {
		return readStatus;
	}
	return MCP4725_PowerControlWithDACValue(hi2c, mode, dacData.dacRegVal);
}

uint16_t MCP4725_getDACVal(int16_t voltage) {
	uint16_t dacVal = voltage / (MCP4725_VDD / 4096.0);
	if (dacVal >= 4096) {
		return 4095;
	} else if (dacVal < 0) {
		return 0;
	} else {
		return dacVal;
	}
}

int16_t MCP4725_getVoltage(uint16_t dacVal) {
	uint16_t voltage = (MCP4725_VDD / 4096.0) * dacVal;
	if (voltage >= MCP4725_VDD) {
		return MCP4725_VDD;
	} else if (voltage < 0) {
		return 0;
	} else {
		return voltage;
	}
}

HAL_StatusTypeDef MCP4725_Reset(I2C_HandleTypeDef hi2c) {
	uint8_t data = 0x06;
	return HAL_I2C_Master_Transmit(&hi2c, MCP4725_GENERALCALL_ADDR, &data, 1, MCP4725_TIMEOUT);
}

HAL_StatusTypeDef MCP4725_PowerDown(I2C_HandleTypeDef hi2c) {
	return MCP4725_PowerControl(hi2c, MCP_PM_100K);
}

HAL_StatusTypeDef MCP4725_WakeUp(I2C_HandleTypeDef hi2c) {
	uint8_t data = 0x09;
	return HAL_I2C_Master_Transmit(&hi2c, MCP4725_GENERALCALL_ADDR, &data, 1, MCP4725_TIMEOUT);
}

HAL_StatusTypeDef MCP4725_CheckI2CSpeed(I2C_HandleTypeDef hi2c) {
	if (hi2c.Init.ClockSpeed > 100000) {
		return HAL_ERROR;
	} else {
		return HAL_OK;
	}
}
