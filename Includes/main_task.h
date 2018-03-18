/**
* @file main_task.h
* @brief Prototype functions to drive the Projct 1
*
* This header file provides the prototypes of the functions
* to drive the multithreading project for interfacing two sensors
*
* @author Sharanjeet Singh Mago
* @date 17 March 2018
*
*/

#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <float.h>
#include <complex.h>
#include <stdint.h>
#include <time.h>       
#include <mqueue.h>
#include "../Includes/light_task.h"
#include "light_task.c"
#include "../Includes/temp_task.h"
#include "temp_task.c"
#include "socket_task.c"

#define HB_PORT_ADR 5000
#define IP_ADR      "127.0.0.1"

pthread_t logger_id, light_id, temp_id, socket_id; 

typedef struct              //structure to be sent
{             
	char timestamp[50];     
	int source_id;
	int data;
	float value;
	char random_string[50];
}mystruct;

struct threadParam
{
char *filename;
};

/**
* @brief Client to send HB data for light sensor
*
*
* @return Error condition
*/
int light_client(void);

/**
* @brief Thread Function for Light Task
*
*/
void *func_light(void);

/**
* @brief Client to send HB data for temperature sensor
*
*
* @return Error condition
*/
int temp_client(void);

/**
* @brief Thread Function for Temperature Task
*
*/
void *func_temp(void);

/**
* @brief Thread Function for Logger Task
*
*/
void* logger_task(void);

/**
* @brief Thread Function for Socket Task
*
*/
void* func_socket(void);

/**
* @brief Server to recieve data from tasks
*
* @return Error condition if status fails
*/
int check_status(void);

/**
* @brief Function to test all the startup tests
*
* @return Error condition if tests fails
*/
int startup_test(void);

#endif