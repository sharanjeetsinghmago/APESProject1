/*
*@Filename:light_task.h
*@Description:This is a header for library for the light sensor apds9301
*@Author:Anay Gondhalekar and Sharanjeet Singh Mago
*@Date: 03/15/2018
*@compiler:gcc
*@Usage : Connect light sensor to I2C and use any of the library function to read and write registers
 */


#ifndef LIGHT_TASK_H_
#define LIGHT_TASK_H_

#define time_high 0x02  //for 402ms
#define time_med 0x01  //for 101ms
#define time_low 0x00  //for 13ms

#define gain 0x10  //for maximum gain

int control_reg_wr ( int fd, int msg);
/**
* @brief Write to Control register of light sensor
*
* @return Error Condition
*/
int control_reg_rd ( int fd);
/**
* @brief Read from Control register of light sensor
*
* @return Error Condition
*/
int timing_reg_wr ( int fd, int msg);
/**
* @brief Write to timing register of light sensor
*
* @return Error Condition
*/
int timing_reg_rd(int fd);
/**
* @brief Write from timing register of light sensor
*
* @return Error Condition
*/
int control_reg_int_wr(int fd, int msg);
/**
* @brief Write to Control register Interupt of light sensor
*
* @return Error Condition
*/
int control_reg_int_rd(int fd);
/**
* @brief Read from Control register Interupt of light sensor
*
* @return Error Condition
*/
int threshold_int_reg_wr(int fd, int *array);
/**
* @brief Write to Threshold register Interupt of light sensor
*
* @return Error Condition
*/
int threshold_int_reg_rd(int fd, int *array);
/**
* @brief Read from Threshold register Interupt of light sensor
*
* @return Error Condition
*/
int id_reg_rd(int fd);
/**
* @brief Read Light sensor id register
*
* @return Error Condition
*/
uint16_t data0_reg_rd(int fd);
/**
* @brief Read Light sensor configuration register
*
* @return Config register value
*/
uint16_t data1_reg_rd(int fd);
/**
* @brief Function to read lux from light sensor
*
* @return lux value
*/
float get_lux(void);

/**
* @brief Function to initialize light sensor
*
* @return Error condition if init fails
*/
int light_init(void);
/**
* @brief Function to read and write all functions
**
* @return Error condition
*/

int all_reg_rd_wr(int fd);

#endif 
