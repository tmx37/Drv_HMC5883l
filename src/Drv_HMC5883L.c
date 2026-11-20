/*
 * Drv_HMC5883L.c
 * @brief   Source file for the HMC5883L 3-Axis Digital Compass driver.
 *
 *  Created on: Mar 19, 2025
 *      Author: tmx37
 */
#include "Drv_HMC5883L_Cfg.h"
#include "Drv_HMC5883L.h"

uint32_t I2cPort = 0;

static Drv_HMC5883L_DelayCB_t mDelayCB;
static Drv_HMC5883L_WriteRegisterCB_t mWriteRegisterCB;
static Drv_HMC5883L_ReadRegisterCB_t mReadRegisterCB;
static Drv_HMC5883L_IsDeviceReadyCB_t mIsDeviceReadyCB;
static Drv_HMC5883L_MasterTransmitCB_t mMasterTrasmitCB;
static Drv_HMC5883L_MasterReceiveCB_t mMasterReceiveCB;

GenUtl_Err_t Drv_HMC5883L_Init(const Drv_HMC5883L_Cfg_t *pConfigData)
{
	I2cPort = pConfigData->I2CPort;
    mDelayCB = pConfigData->pfDelayCB;
    mWriteRegisterCB = pConfigData->pfWriteRegistersI2cCB;
    mReadRegisterCB = pConfigData->pfReadRegistersI2cCB;
    mIsDeviceReadyCB = pConfigData->pfIsDeviceReadyI2cCB;
    mMasterTrasmitCB = pConfigData->pfMasterTransmitI2cCB;
    mMasterReceiveCB = pConfigData->pfMasterReveiceCB;

    // Device time to be ready for I2C commands
    mDelayCB(260);

    return OUT_OK;
}


/**
 * @brief  Sends data to a register on the HMC5883L via I2C.
 *
 * @param address     I2C address of the HMC5883L (read/write).
 * @param reg_address Register address to write to.
 * @param hmc_data    Pointer to data buffer to send.
 * @param size        Number of bytes to write.
 */
void HMC_SEND_DATA(hmc_adr_t address, hmc_reg_adr_t reg_address, uint8_t *hmc_data, uint16_t size) 
{
	mWriteRegisterCB(address, reg_address, hmc_data, size);
}

/**
 * @brief  Reads magnetic field data from the X, Y, and Z axes.
 *
 * @param compass Pointer to an hmc_t structure to store the data.
 * @note   Make sure a measurement has been triggered before calling this function.
 */
void HMC_Read_Compass(hmc_t *compass) 
{
	uint8_t data[6];
	mReadRegisterCB(HMC_READ_ADR, HMC_X_MSB, data, 6);
	compass->compass_x = (int16_t) ((data[0] << 8) | data[1]);
	compass->compass_y = (int16_t) ((data[2] << 8) | data[3]);
	compass->compass_z = (int16_t) ((data[4] << 8) | data[5]);
}

/**
 * @brief  Reads the current measurement mode from the Mode register.
 *
 * @param compass Pointer to an hmc_t structure to update the mode field.
 */
void HMC_Read_MEASUR_MODE(hmc_t *compass) {
	uint8_t hmc_data;
	mReadRegisterCB(HMC_READ_ADR, HMC_MODE_REG, &hmc_data, 1);
	compass->mode = (hmc_measurement_mode_t) (hmc_data & 0x03);
}

/**
 * @brief  Sets the measurement mode in the Mode register.
 *
 * @param mode Measurement mode to set (e.g. single, continuous).
 */
void HMC_Set_MEASUR_MODE(hmc_measurement_mode_t mode) {
	uint8_t hmc_data = mode;
	HMC_SEND_DATA(HMC_WRITE_ADR, HMC_MODE_REG, &hmc_data, 1);
}

/**
 * @brief  Reads Configuration Register A and updates related fields in the struct.
 *
 * @param compass Pointer to an hmc_t structure to update averaging, datarate and bias.
 */
void HMC_Read_Reg_A(hmc_t *compass) {
	uint8_t hmc_data;
	mReadRegisterCB(HMC_READ_ADR, HMC_CON_REG_A, &hmc_data, 1);
	compass->averaging = (hmc_avg_t) ((hmc_data >> 5) & 0x03);
	compass->datarate = (hmc_datarate_t) ((hmc_data >> 2) & 0x07);
	compass->bias = (hmc_bias_t) (hmc_data & 0x03);
}

/**
 * @brief  Sets averaging, output data rate and bias settings in Configuration Register A.
 *
 * @param avg   Sample averaging setting.
 * @param dr    Output data rate setting.
 * @param bias  Measurement bias setting.
 */
void HMC_Set_Reg_A(hmc_avg_t avg, hmc_datarate_t dr, hmc_bias_t bias) {
	uint8_t hmc_data = 0;
	hmc_data |= ((avg & 0x03) << 5);
	hmc_data |= ((dr & 0x07) << 2);
	hmc_data |= (bias & 0x03);
	mWriteRegisterCB(HMC_READ_ADR, HMC_CON_REG_A, &hmc_data, 1);
}

/**
 * @brief  Reads gain setting from Configuration Register B.
 *
 * @param compass Pointer to an hmc_t structure to update gain field.
 */
void HMC_Read_Gain(hmc_t *compass) {
	uint8_t hmc_data;
	mReadRegisterCB(HMC_READ_ADR, HMC_CON_REG_B, &hmc_data, 1);
	compass->gain = (hmc_gain_t) (hmc_data & 0xE0);
}

/**
 * @brief  Sets gain by writing to Configuration Register B.
 *
 * @param gain Gain setting to write.
 */
void HMC_Set_Gain(hmc_gain_t gain) {
	uint8_t hmc_data = gain;
	HMC_SEND_DATA(HMC_WRITE_ADR, HMC_CON_REG_B, &hmc_data, 1);
}

/**
 * @brief  Triggers a single measurement (used in single measurement mode).
 *
 * @note This does not read any data, it only initiates measurement.
 */
void HMC_Make_Measurement_4_Single(void) {
	HMC_Set_MEASUR_MODE(HMC_SING_MEAS);
}

/**
 * @brief  Initializes the sensor with default configuration values.
 *
 * @note Uses defaults from HMC5883L.h (gain, averaging, datarate, bias, mode).
 */
void HMC5883L_Init(void) {
	HMC_Set_Reg_A(HMC_DEFAULT_AVERAGING, HMC_DEFAULT_DATARATE,
			HMC_DEFAULT_BIAS);
	HMC_Set_Gain(HMC_DEFAULT_GAIN);
	HMC_Set_MEASUR_MODE(HMC_DEFAULT_MEASUREMENT_MODE);
}

/**
 * @brief  Reads all sensor configuration and magnetic field values.
 *
 * @param compass Pointer to an hmc_t structure to store all values.
 */
void HMC_Read_All(hmc_t *compass) {
	HMC_Read_Compass(compass);
	HMC_Read_MEASUR_MODE(compass);
	HMC_Read_Reg_A(compass);
	HMC_Read_Gain(compass);
}

/**
 * @brief  Verifies sensor identity by checking the ID registers.
 *
 * @retval HAL_OK    If the ID matches (0x48, 0x34, 0x33)
 * @retval HAL_ERROR If the ID does not match or read fails.
 */
GenUtl_Err_t HMC5883L_Check_ID(void) {
	uint8_t id[3];

	if (mReadRegisterCB(HMC_READ_ADR, 0x0A, id,3) != OUT_OK)
		return OUT_ERR_NOT_FOUND;

	if (id[0] == 0x48 && id[1] == 0x34 && id[2] == 0x33)
		return OUT_OK;
	else
		return OUT_ERR_INVALID_VERSION;
}
