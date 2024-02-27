#include <stdlib.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"


extern volatile WarpI2CDeviceState	deviceINA219State;
extern volatile uint32_t		gWarpI2cBaudRateKbps;
extern volatile uint32_t		gWarpI2cTimeoutMilliseconds;
extern volatile uint32_t		gWarpSupplySettlingDelayMilliseconds;


WarpStatus
writeSensorRegisterINA219(uint8_t deviceRegister, uint16_t payload)
{
	uint8_t		commandByte[1];
	uint8_t		payloadByte[1];
	i2c_status_t	status;

	switch (deviceRegister)
	{
		case 0x00: case 0x05:
		{
			/* OK */
			break;
		}

		default:
		{
			return kWarpStatusBadDeviceCommand;
		}
	}

	i2c_device_t slave =
		{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	warpScaleSupplyVoltage(deviceINA219State.operatingVoltageMillivolts);
	commandByte[0] = deviceRegister;
	payloadByte[0] = payload;
	payloadByte[1] = (payload >> 8);
	warpEnableI2Cpins();

	status = I2C_DRV_MasterSendDataBlocking(
		0 /* I2C instance */,
		&slave,
		commandByte,
		1,
		payloadByte,
		2,
		gWarpI2cTimeoutMilliseconds);
	if (status != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}

WarpStatus
configureSensorINA219(uint16_t payloadF_SETUP, uint16_t payloadCALIB_REG)
{
	WarpStatus	i2cWriteStatus1, i2cWriteStatus2;


	warpScaleSupplyVoltage(deviceINA219State.operatingVoltageMillivolts);

	i2cWriteStatus1 = writeSensorRegisterINA219(kWarpSensorConfigurationRegisterINA219F_SETUP /* configuration register address */,
												  payloadF_SETUP /* payload: Disable FIFO */
	);
	
	SEGGER_RTT_WriteString(0, "\r\n\tINA219 configuration loaded\n");
	
	i2cWriteStatus2 = writeSensorRegisterINA219(kWarpSensorConfigurationRegisterINA219CALIB_REG /* calibration register address */,
												  payloadCALIB_REG /* payload */
	);
	
	SEGGER_RTT_WriteString(0, "\r\n\tINA219 Calibration done\n");
	
	return (i2cWriteStatus1 | i2cWriteStatus2);
}

WarpStatus
readSensorRegisterINA219(uint8_t deviceRegister, int numberOfBytes)
{
	uint8_t		cmdBuf[1] = {0xFF};
	i2c_status_t	status;


	USED(numberOfBytes);
	switch (deviceRegister)
	{
		case 0x00: case 0x01:case 0x02: case 0x03: 
		case 0x04: case 0x05:
		{
			/* OK */
			break;
		}

		default:
		{
			return kWarpStatusBadDeviceCommand;
		}
	}

	i2c_device_t slave =
		{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	warpScaleSupplyVoltage(deviceINA219State.operatingVoltageMillivolts);
	cmdBuf[0] = deviceRegister;
	warpEnableI2Cpins();

	status = I2C_DRV_MasterReceiveDataBlocking(
		0 /* I2C peripheral instance */,
		&slave,
		cmdBuf,
		1,
		(uint8_t *)deviceINA219State.i2cBuffer,
		numberOfBytes,
		gWarpI2cTimeoutMilliseconds);

	if (status != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}

void
printSensorDataINA219(bool hexModeFlag)
{
	uint8_t		readSensorRegisterValueLSB;
	uint8_t		readSensorRegisterValueMSB;
	int16_t		readSensorRegisterValueCombined;
	WarpStatus	i2cReadStatus;


	warpScaleSupplyVoltage(deviceINA219State.operatingVoltageMillivolts);

	/*
	 *	Read INA219 current sensor data:
	 */
	i2cReadStatus = readSensorRegisterINA219(kWarpSensorOutputRegisterINA219_Current, 2 /* numberOfBytes */);
	readSensorRegisterValueMSB = deviceINA219State.i2cBuffer[0];
	readSensorRegisterValueLSB = deviceINA219State.i2cBuffer[1];
	readSensorRegisterValueCombined = (readSensorRegisterValueMSB) | (readSensorRegisterValueLSB);

	if (i2cReadStatus != kWarpStatusOK)
	{
		warpPrint(" ----,");
	}
	else
	{
		if (hexModeFlag)
		{
			warpPrint(" 0x%02x 0x%02x,", readSensorRegisterValueMSB, readSensorRegisterValueLSB);
		}
		else
		{
			warpPrint(" %d,", readSensorRegisterValueCombined);
		}
	}
}

uint8_t
appendSensorDataINA219(uint8_t* buf)
{
	uint8_t index = 0;
	uint8_t readSensorRegisterValueLSB;
	uint8_t readSensorRegisterValueMSB;
	int16_t readSensorRegisterValueCombined;
	WarpStatus i2cReadStatus;

	warpScaleSupplyVoltage(deviceINA219State.operatingVoltageMillivolts);

	/*
	 *	Read INA219 current sensor data:
	 */
	i2cReadStatus                   = readSensorRegisterINA219(kWarpSensorOutputRegisterINA219_Current, 2 /* numberOfBytes */);
	readSensorRegisterValueMSB      = deviceINA219State.i2cBuffer[0];
	readSensorRegisterValueLSB      = deviceINA219State.i2cBuffer[1];
	readSensorRegisterValueCombined = (readSensorRegisterValueMSB) | (readSensorRegisterValueLSB);


	if (i2cReadStatus != kWarpStatusOK)
	{
		buf[index] = 0;
		index += 1;

		buf[index] = 0;
		index += 1;
	}
	else
	{
		/*
		 * MSB first
		 */
		buf[index] = (uint8_t)(readSensorRegisterValueCombined >> 8);
		index += 1;

		buf[index] = (uint8_t)(readSensorRegisterValueCombined);
		index += 1;
	}
	return index;
}

void
initINA219(const uint8_t i2cAddress, uint16_t operatingVoltageMillivolts)
{
	deviceINA219State.i2cAddress			= i2cAddress;
	deviceINA219State.operatingVoltageMillivolts	= operatingVoltageMillivolts;
	configureSensorINA219(14751, 4096);

	return;
}
