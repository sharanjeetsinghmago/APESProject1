/*
*@Filename:temp_task.h
*@Description:This is a header for library for the temp sensor tmp102
*@Author:Anay Gondhalekar and Sharanjeet Singh Mago
*@Date: 03/15/2018
*@compiler:gcc
*@Usage : Connect temperature sensor to I2C and use any of the library function to read and write registers
 */

#ifndef _TEMP_TASK_H_
#define _TEMP_TASK_H_

/**
* @brief Function to initialize temperature sensor
*
* @return Error condition if init fails
*/
int temp_init();

/**
* @brief Function to read from temperature data register
*
* @param Unit to get the data (0=Celcius,1=Kelvin,2=Fahrenheit)
*
* @return Converted data or error if conversion fails
*/
float read_temp_data_reg(int unit);

/**
* @brief Function to read and write all functions
**
* @return Error condition
*/
int all_temprg_rd_wr();

/**
* @brief Read Temperature sensor configuration register
*
* @return Config register value
*/
uint16_t read_temp_config_register();

/**
* @brief Read Tlow and Thigh register
*
*
* @param Address of register you want to read
*
* @return Value of the register
*/
uint16_t read_tlow_reg(int reg);

/**
* @brief Function to write config register
*
* @return Error condition
*/
int write_config_register_default();

/**
* @brief Function to set conversion rate from config register
*
* @param value to write
*
* @return Error condition
*/
int write_config_reg_conv_rate(uint8_t value );

/**
* @brief Function to write to config register
*
* @param value to write to config register
*
* @return Error condition
*/
int write_config_reg_em(uint8_t value );

/**
* @brief Function to write On or Off to config register
*
* @param value to write to config register (0 0r 1)

* @return Error condition
*/
int write_config_reg_on_off(uint8_t value );

/**
* @brief Function to write Tlow or Thigh register
*
* @param reg Address of Thigh or Tlow register
* @param calue Value to write to register
*
* @return Error condition
*/
int write_tlow_reg(int reg, uint16_t value );

/**
* @brief Function to write pointer register
*
* @param Value to write to pointer register
*
* @return Error condition
*/
void write_pointer_reg(uint8_t value);

#endif
