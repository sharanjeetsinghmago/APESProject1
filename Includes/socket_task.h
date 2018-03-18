/**
* @file socket_task.h
* @brief Header File for Server of the socket task
*
*
* @author Sharanjeet Singh Mago
* @date 16 March 2018
*
*/


#ifndef __SOCKET_TASK_H__
#define __SOCKET_TASK_H__

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT_ADR    2000

typedef struct
{
  char    buf[20];
  int  buf_len;
  bool    usrLED_OnOff;
}payload_t;

/**
* @brief Creates a server for and external client to connect and get data
* 
* @return Error siganl is the socket fails
*/
int socket_task(void);

#endif /* __SOCKET_TASK_H__ */