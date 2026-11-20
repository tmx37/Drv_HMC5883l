/*
 * Drv_HMC5883L_Cfg.c
 *
 *  Created on: Mar 19, 2025
 *      Author: tmx37
 */

#include "Drv_HMC5883L_Cfg.h"
#include "Drv_HMC5883L.h"

static Drv_HMC5883L_Cfg_t mConfigData;

uint32_t MockWriteI2CReg(uint32_t I2cInst, uint8_t DevAddr, uint8_t RegNdx, uint8_t *pDataBuffer, uint32_t DataSize);
uint32_t MockReadI2CReg(uint32_t I2cInst, uint8_t DevAddr, uint8_t RegNdx, uint8_t *pDataBuffer, uint32_t DataSize);
uint32_t MockIsDeviceReady(uint32_t I2cInst, uint8_t DevAddr, uint8_t nTrials);

uint32_t MockMasterTransmitI2C(uint32_t I2cInst, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
uint32_t MockMasterReceiveI2C(uint32_t I2cInst, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

uint32_t MockDelay(uint32_t Delay);


GenUtl_Err_t Drv_HMC5883LCfg_Init()
{
    GenUtl_Err_t ErrCode;
    mConfigData.pfWriteRegistersI2cCB = MockWriteI2CReg;
    mConfigData.pfReadRegistersI2cCB = MockReadI2CReg;
    mConfigData.pfIsDeviceReadyI2cCB = MockIsDeviceReady;
    mConfigData.pfMasterTransmitI2cCB = MockMasterTransmitI2C;
    mConfigData.pfMasterReveiceCB = MockMasterReceiveI2C;
    mConfigData.pfDelayCB = MockDelay;

    ErrCode = Drv_HMC5883L_Init(&mConfigData);
    return ErrCode;
}

uint32_t MockWriteI2CReg(uint32_t I2cInst, uint8_t DevAddr, uint8_t RegNdx, uint8_t *pDataBuffer, uint32_t DataSize)
{
    return OUT_OK;
}

uint32_t MockReadI2CReg(uint32_t I2cInst, uint8_t DevAddr, uint8_t RegNdx, uint8_t *pDataBuffer, uint32_t DataSize)
{
    return OUT_OK;
}

uint32_t MockMasterTransmitI2C(uint32_t I2cInst, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    return OUT_OK;
}

uint32_t MockMasterReceiveI2C(uint32_t I2cInst, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    return OUT_OK;
}

uint32_t MockIsDeviceReady(uint32_t I2cInst, uint8_t DevAddr, uint8_t nTrials)
{
    return OUT_OK;
}

uint32_t MockDelay(uint32_t Delay)
{
    (void)Delay;
}