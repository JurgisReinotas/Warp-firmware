#include <stdlib.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"

static int
regReadINA219(uint16_t registerToRead, int numberOfBytes)
{
	i2c_status_t	status;
  
	i2c_device_t slave =
	{
		.address = 0x40,
		.baudRate_kbps = 300
	};

	status = I2C_DRV_MasterReceiveDataBlocking(
		0 /* I2C peripheral instance */,
		&slave,
		NULL,
		0,
		registerToRead,
		numberOfBytes,
		5);
  
  /*!
    NULL for a command
    0 for zero bytes
    registerToRead - current register address
    numberOfBytes for number of bytes of data
  */

	if (status != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}

static int
regWriteINA219(uint16_t regAddress, uint16_t valueToWrite, int numberOfBytes)
{
	i2c_status_t	status;
  
	i2c_device_t slave =
	{
		.address = 0x40,
		.baudRate_kbps = 300
	};

	status = I2C_DRV_MasterSendDataBlocking(
		0 /* I2C peripheral instance */,
		&slave,
		regAddress,
		2,
		&valueToWrite,
		numberOfBytes,
		5);
  
  /*!
    regAddress for a register address on the INA219
    2 for two bytes
    &valueToWrite - pointer to the register of the value to write
    numberOfBytes - number of bytes of data
  */

	if (status != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}


int
devINA219init(void)
{
  	warpEnableI2Cpins();

	SEGGER_RTT_WriteString(0, "\r\n\tSet the calibration value on INA219\n");
	
	regWriteINA219(0x05, 4096, 2);
	
	SEGGER_RTT_WriteString(0, "\r\n\tDone setting the calibration value on INA219\n");
	SEGGER_RTT_WriteString(0, "\r\n\tRead current register on INA219\n");
	
	regReadINA219(0x04, 2);
	
	SEGGER_RTT_WriteString(0, "\r\n\tDone reading the current register on INA219\n");
  	
	return 0;
}
