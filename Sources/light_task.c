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
#include "lightfunc.h"
#include <time.h>

int control_reg_wr ( int fd, int msg)
{
int temp = 0x80;                      //for control register
if(write(fd,&temp,1) != 1)
{
printf("Error in writing to control register\n");
return -1;
}

if(write(fd,&msg,1) != 1)
{
printf("Error in writing message to control register\n");
return -1;
}
return 0;
}

int control_reg_rd ( int fd)
{
int temp = 0x80 ;                     //for control register
if(write(fd,&temp,1) != 1)
{
printf("Error in writing from control register\n");
return -1;
}

if(read(fd,&temp,1) != 1)            
{
printf("Error in reading message from control register\n");
return -1;
}
return temp;
}

int timing_reg_wr ( int fd, int msg)
{
int temp = 0x81  ;                    //for timing register
if(write(fd,&temp,1) != 1)
{
printf("Error in writing to control register\n");
return -1;
}

if(write(fd,&msg,1) != 1)
{
printf("Error in writing message to control register\n");
return -1;
}
return 0;
}

int timing_reg_rd(int fd){
int temp =  0x81;                    //for timing register
if( write(fd, &temp, 1) != 1)
{
printf("Error in writing to control register\n");
return -1;
}

if( read(fd, &temp, 1) != 1)
{
printf("Error in writing message to control register\n");
return -1;
}

return temp;
}


int control_reg_int_wr(int fd, int msg)
{
int temp = 0x86;                    //for interrupt control register
if( write(fd, &temp, 1) != 1)
{
printf("Error in writing to interrupt control register\n");
return -1;
}

if( write(fd, &msg, 1) != 1)
{
printf("Error in writing message to interrupt control register\n");
return -1;
}
return 0;
}

/*read from interrupt control register */
int control_reg_int_rd(int fd)
{
int temp =  0x86;
if( write(fd, &temp, 1) != 1)
{
printf("Error in writing to control interrupt register\n");
return -1;
}
if( read(fd, &temp, 1) != 1)
{
printf("Error in reading from control interrupt register\n");
return -1;
}

return temp;
}

int threshold_int_reg_wr(int fd, int *array){

	int temp = 0x82;                              //for threshold low-low to threshold high-high
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	temp = array[0];
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}

	temp = 0x83;
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	temp = array[1];
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}

	temp = 0x84;
	if( write(fd, &temp, 1) != 1){
		printf("Unable write to threshhold interrupt register\n");
		return -1;
	}
	temp = array[2];{
	if( write(fd, &temp, 1) != 1)
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	
	temp = 0x85;
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	temp = array[3];
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	return 0;

}

/*Read value from interrupt_threshhold register
 * Returns either read value and fail on failure
 */
int threshold_int_reg_rd(int fd, int *array){

	int temp =  0x82 ;                          //for threshold low-low to threshold high-high
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	if( read(fd, &temp, 1) != 1){
		printf("Unable to read from threshhold interrupt register\n");
		return -1;
	}
	array[0] = temp;

	temp =  0x83;
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	if( read(fd, &temp, 1) != 1){
		printf("Unable to read from threshhold interrupt register\n");
		return -1;
	}
	array[1] = temp;

	temp =  0x84 ;
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	if( read(fd, &temp, 1) != 1){
		printf("Unable to read from threshhold interrupt register\n");
		return -1;
	}
	array[2] = temp;

	temp =  0x85 ;
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to threshhold interrupt register\n");
		return -1;
	}
	if( read(fd, &temp, 1) != 1){
		printf("Unable to read from threshhold interrupt register\n");
		return -1;
	}
	array[3] = temp;
	return 0;

}

int id_reg_rd(int fd)
{
	int temp =  0x8A;                             //for id register
	if( write(fd, &temp, 1) != 1){
		printf("Unable to write to id register\n");
		return -1;
	}
	if( read(fd, &temp, 1) != 1){
		printf("Unable to read from id register\n");
		return -1;
	}

	return temp;
}

uint16_t data0_reg_rd(int fd){
	int temp =  0x8C;

	if( write(fd, &temp, 1) != 1)
	{
		printf("Unable to write to data 0 register\n");
		return -1;
	}
	uint8_t dlow0;

	if( read(fd, &dlow0, 1) != 1)
	{
		printf("Unable to read from data 0 register\n");
		return -1;
	}

	temp =  0x8D;
	if( write(fd, &temp, 1) != 1)
	{
		printf("Unable to write to data 0 register\n");
		return -1;
	}
	uint16_t dhigh0;

	if( read(fd, &dhigh0, 1) != 1)
	{
		printf("Unable to read from write 0 register\n");
		return -1;
	}


	uint16_t final = dhigh0<<8 | dlow0;
	return final;
}

uint16_t data1_reg_rd(int fd){
	int temp =  0x8E;

	if( write(fd, &temp, 1) != 1)
	{
		printf("Unable to write to data 1 register\n");
		return -1;
	}
	uint8_t dlow1;

	if( read(fd, &dlow1, 1) != 1)
	{
		printf("Unable to read from data 1 register\n");
		return -1;
	}

	temp =  0x8F;
	if( write(fd, &temp, 1) != 1)
	{
		printf("Unable to write to data 1 register\n");
		return -1;
	}
	uint16_t dhigh1;

	if( read(fd, &dhigh1, 1) != 1)
	{
		printf("Unable to read from write 1 register\n");
		return -1;
	}


	uint16_t final = dhigh1<<8 | dlow1;
	return final;
}

int light_init(int number)
{
	int file;
	char myfile[20];

	snprintf(myfile, 18, "/dev/i2c-%d", number);
	file = open(myfile, O_RDWR);
	if (file < 0)
	 {
		printf("Unable to open the i2c file.\n");
		return -1;
	}
	int addr = 0x39; //The I2C slave address

	if (ioctl(file, I2C_SLAVE, addr) < 0)
	 {
		printf("Unable to use ioctl call.\n");
		return -1;
	}
	return file;
}

float get_lux(int fd)
{
	float ch_0 = 0, ch_1 = 0;
	float adc,lux;

	control_reg_wr(fd, 0x03); //to power up the sensor
	timing_reg_wr(fd, time_high|gain);

	usleep(5000);

	ch_0 = (float)data0_reg_rd(fd);
	ch_1 = (float)data1_reg_rd(fd);

	adc = ch_1/ch_0;

	/*As per datasheet*/
	if(adc>0 && adc <= 0.5)
	return lux = (0.0304 * ch_0) - (0.062 * ch_0 * pow(adc, 1.4));

	else if((adc>0.5) && (adc<=0.61))
	return lux = (0.0224 * ch_0) - (0.031 * ch_1);

    	else if((adc>0.61)&&(adc<=0.80))
        return lux = (0.0128 * ch_0) - (0.0153 * ch_1);

    	else if((adc>0.80) && (adc<=1.30))
        return lux = (0.00146 * ch_0) - (0.00112 * ch_1);

    	else if(adc > 1.30)
        return lux = 0;

return -1;
}

int main()
{int file;
file = light_init(2);
time_t curtime;
time(&curtime);
while(1)
{
float lumen = get_lux(file);
printf("Time stamp: %s",ctime(&curtime));
printf("Length of time stamp : %ld",strlen(ctime(&curtime)));
printf("The current lux is %f\n", lumen);  
}
return 0;
}
