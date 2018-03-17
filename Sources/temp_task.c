#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>

int fd;
char *bus = "/dev/i2c-2";     /* Pins P9_19 and P9_20 */
int addr = 0x48;              /* The I2C address of TMP102 */
char buf[2] = {0},pika=0x00;
int temp;
unsigned char MSB, LSB;
float f,c,k;

typedef enum 
{
  TEMP_CEL = 0,
  TEMP_KEL = 1,
  TEMP_FAH = 2
}temp_format;

int default_config_byte_one = 0X50;
int default_config_byte_two = 0XA0;

void write_pointer_reg(uint8_t value)
{
  if(write(fd, &value, 1) != 1)
  {
    perror("Write Pointer Register Error\n");
  }
}

void write_tlow_reg(int reg, uint16_t value )
{  
  write_pointer_reg(reg);
  
  if (write(fd, &value, 2) != 2) 
  {
    perror("T-low register write error");
  }
}


void write_config_reg_on_off(uint8_t value )
{
  write_pointer_reg(0b00000001);
  if((value == 0) || (value == 1))
  {
    default_config_byte_one |= value;
    
    if (write(fd, &default_config_byte_one, 1) != 1) 
    {
      perror("Configuration register write error for first byte");
    }
    
    if (write(fd, &default_config_byte_two, 1) != 1) 
    {
      perror("Configuration register write error for second byte");
    }
  }
}

void write_config_reg_em(uint8_t value )
{ 
  write_pointer_reg(0b00000001);
  if((value == 0) || (value == 1))
  {
    default_config_byte_two |= (value << 4);
    
    if (write(fd, &default_config_byte_one, 1) != 1) 
    {
      perror("Configuration register write error for first byte");
    }
    
    if (write(fd, &default_config_byte_two, 1) != 1) 
    {
      perror("Configuration register write error for second byte");
    }
  }
}

void write_config_reg_conv_rate(uint8_t value )
{
  write_pointer_reg(0b00000001);
  if((value >= 0) || (value <= 3))
  {
    default_config_byte_two |= (value << 6);
    if (write(fd, &default_config_byte_one, 1) != 1) 
    {
      perror("Configuration register write error for first byte");
    }
    if (write(fd, &default_config_byte_two, 1) != 1) 
    {
      perror("Configuration register write error for second byte");
    }
  }
}

void write_config_register_default( )
{  
  write_pointer_reg(0b00000001);
  if (write(fd, &default_config_byte_one, 1) != 1) 
  {
    perror("Configuration register write error for first byte");
  } 
  if (write(fd, &default_config_byte_two, 1) != 1) 
  {
    perror("Configuration register write error for second byte");
  }
}



uint16_t read_tlow_reg(int reg)
{
  uint16_t value;
  uint8_t v[1]={0};
  write_pointer_reg(reg);
  if (read(fd, v, 1) != 1) 
  {
    perror("T-low register read error");
  }
  value = (v[0]<<4 | (v[1] >> 4 & 0XF));
  printf("T-low register value is: %f \n", value);
  return value; 
}

uint16_t read_temp_config_register()
{
  uint16_t value;
  uint8_t v[1]={0};
  write_pointer_reg(0b00000001);
  if (read(fd, v, 1) != 1) 
  {
    perror("Temperature configuration register read error");
  }
  value = (v[0]<<8 | v[1]);
  printf("Temperature configuration register value is: %f \n", value);
  return value;
}

float read_temp_data_reg(int unit)
{

  write_pointer_reg(0b00000000);

  int x = read(fd,&buf,2);

  //printf("number of bytes read = %d\n",x);

  if (x != 2) 
  {
    /* ERROR HANDLING: i2c transaction failed */
    perror("Failed to read from the i2c bus.\n");
    printf("ERROR : %s\n", strerror(errno));
  }
  else 
  {

    MSB = buf[0];
    LSB = buf[1];

    /* Convert 12bit int using two's compliment */
    temp = ((MSB << 8) | LSB) >> 4;

    c = temp*0.0625;
    f = (1.8 * c) + 32;
    k = c + 273.15;
    //printf("Temp Fahrenheit: %f Celsius: %f\n", f, c);
   
    if(unit == TEMP_CEL)
      return c;
    else if(unit == TEMP_KEL)
      return k;
    else if(unit == TEMP_FAH)
      return f;
  }
}

int temp_init()
{
  if((fd = open(bus, O_RDWR)) < 0)
  {
    perror("Failed to open the i2c bus");
    /* ERROR HANDLING: you can check errno to see what went wrong */
    return -1;
  }

  if(ioctl(fd, I2C_SLAVE, addr) < 0)
  {
    perror("Failed to open the i2c bus");
    /* ERROR HANDLING: you can check errno to see what went wrong */
    return -1;
  }

  return 0;
}
