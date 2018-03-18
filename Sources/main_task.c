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

char file_name[50];

typedef struct              //structure to be sent
{             
char timestamp[50];     
int source_id;
int log_level;
int data;
float value;
char random_string[50];
}mystruct;

struct threadParam
{
char *filename;
};

int light_client()
{
  int client_socket = 0;
  struct sockaddr_in serv_addr = {0};
  const char* msg = "Light Alive";
  payload_t ploadSend;
  int sent_b;
  size_t pload_size;
  char r_data[4] = {0};

  /* Enter the message into payload structure */
  memcpy(ploadSend.buf,msg,strlen(msg)+1);
  ploadSend.buf_len = strlen(ploadSend.buf);
  ploadSend.usrLED_OnOff = 1;

  /* create socket */
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    //printf("[Client] [ERROR] Socket creation Error\n");
    return -1;
  }
  else
    //printf("[Client] Socket Created Successfully\n");

  /* Fill the socket address structure */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(HB_PORT_ADR);
      
  /* convert the IP ADDR to proper format */
  if(inet_pton(AF_INET, IP_ADR, &serv_addr.sin_addr)<=0) 
  {
    //printf("[Client] [ERROR] Address Conversion Error\n");
    return -1;
  }
  
  /* connect the socket before sending the data */
  if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    //printf("[Client] [ERROR] Connection Failed \n");
    return -1;
  }

  /*send the size of the incoming payload */
  pload_size = sizeof(ploadSend);
  sent_b = send(client_socket,&pload_size,sizeof(size_t), 0);
  //printf("[Client] Sent payload size: %d\n", pload_size);

  /*Sending the payload */
  sent_b = send(client_socket , (char*)&ploadSend , sizeof(ploadSend), 0 );
  /* check whether all the bytes are sent or not */
  if(sent_b < sizeof(ploadSend))
  {
    //printf("[Client] [ERROR] Complete data not sent\n");
    return 1;
  }
  
  /* display the date sent */
  //printf("[Client] Message sent from Client\n{\n Message: %s\n MessageLen: %d\n USRLED: %d\n}\n", \
                           ploadSend.buf, ploadSend.buf_len, ploadSend.usrLED_OnOff);
  
  /* read data sent by server */
  //read(client_socket, r_data, 4);
  //printf("[Client]  Message received from Server: %s\n",r_data);

  /* close socket */ 
  close(client_socket);
  
  //return 0;

}

void *func_light()
{       mqd_t mq1;
        printf("[Light Thread] Light Thread Started\n");
        mystruct lightmsg;
        time_t curtime;
        int x = light_init();
        if(x != -1)
        {
          mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
          time(&curtime);
          memcpy(lightmsg.timestamp,ctime(&curtime), strlen(ctime(&curtime)));
          memcpy(lightmsg.random_string,"Light task initiated",20);
          lightmsg.log_level = 0;
          lightmsg.source_id = 1;
          lightmsg.data =1;
          mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);      
	        while(1)
          {   
            //time_t curtime;
            time(&curtime);
            float lumen = get_lux();
                   
            if(lumen < 0 )
        		{

         	  	time(&curtime);
             	memcpy(lightmsg.random_string,"Error in getting data from light task",strlen("Error in getting data from light task"));
            	memcpy(lightmsg.timestamp,ctime(&curtime),24);
             	lightmsg.source_id = 1;
                lightmsg.log_level = 2;
             	lightmsg.data = 1;
        		  mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);
              sleep(1);
         		}
            else
            {    
             memcpy(lightmsg.random_string,"Light data obtained",19);
        		  memcpy(lightmsg.timestamp,ctime(&curtime),24);
              lightmsg.source_id = 1;
              lightmsg.data = 1;
	      lightmsg.log_level = 1;
              mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);

              //printf("The current lux is %f\n", lumen);
              memcpy(lightmsg.timestamp,ctime(&curtime), strlen(ctime(&curtime)));
              lightmsg.data = 0;
              lightmsg.value = lumen; 
              if( mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1)== -1)
              {
                printf("Sending failed\n");
              } 
              sleep(1);
              light_client();
            //exit(0);
            }
          }
        }	
        else 
        {
          printf("[Light Thread] Error initialising light task\n");
          time(&curtime);
          memcpy(lightmsg.random_string,"Error in initialising light task",strlen("Error in initialising light task"));
          memcpy(lightmsg.timestamp,ctime(&curtime),24);
          lightmsg.source_id = 2;
          lightmsg.data = 1;
	  lightmsg.log_level=2;
        	mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);
        }


        printf("[Light Thread] Light Thread Finished\n");
	      time(&curtime);
        memcpy(lightmsg.random_string,"Light task finished",19);
        memcpy(lightmsg.timestamp,ctime(&curtime),24);
        lightmsg.source_id = 2;
        lightmsg.data = 1;
	lightmsg.log_level= 1;
      	mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);

}



int temp_client()
{
  int client_socket = 0;
  struct sockaddr_in serv_addr = {0};
  const char* msg = "Temp Alive";
  payload_t ploadSend;
  int sent_b;
  size_t pload_size;
  char r_data[4] = {0};

  /* Enter the message into payload structure */
  memcpy(ploadSend.buf,msg,strlen(msg)+1);
  ploadSend.buf_len = strlen(ploadSend.buf);
  ploadSend.usrLED_OnOff = 1;

  /* create socket */
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    //printf("[Client] [ERROR] Socket creation Error\n");
    return -1;
  }
  else
    //printf("[Client] Socket Created Successfully\n");

  /* Fill the socket address structure */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(HB_PORT_ADR);
      
  /* convert the IP ADDR to proper format */
  if(inet_pton(AF_INET, IP_ADR, &serv_addr.sin_addr)<=0) 
  {
    //printf("[Client] [ERROR] Address Conversion Error\n");
    return -1;
  }
  
  /* connect the socket before sending the data */
  if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    //printf("[Client] [ERROR] Connection Failed \n");
    return -1;
  }

  /*send the size of the incoming payload */
  pload_size = sizeof(ploadSend);
  sent_b = send(client_socket,&pload_size,sizeof(size_t), 0);
  //printf("[Client] Sent payload size: %d\n", pload_size);

  /*Sending the payload */
  sent_b = send(client_socket , (char*)&ploadSend , sizeof(ploadSend), 0 );
  /* check whether all the bytes are sent or not */
  if(sent_b < sizeof(ploadSend))
  {
    //printf("[Client] [ERROR] Complete data not sent\n");
    return 1;
  }
  
  /* display the date sent */
  //printf("[Client] Message sent from Client\n{\n Message: %s\n MessageLen: %d\n USRLED: %d\n}\n", \
                           ploadSend.buf, ploadSend.buf_len, ploadSend.usrLED_OnOff);
  
  /* read data sent by server */
  //read(client_socket, r_data, 4);
  //printf("[Client]  Message received from Server: %s\n",r_data);

  /* close socket */ 
  close(client_socket);
  
  //return 0;

}

void *func_temp()
{
        mqd_t mq1;
        printf("[Temperature Thread] Temperature Thread Started\n");
        mystruct tempmsg;
        time_t curtime;
        char buffer[50] = {0};
        if(temp_init() != -1)
		    {
                          mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
                          memcpy(tempmsg.random_string,"Temperature task initiated",26);
                          time(&curtime);
                          memcpy(tempmsg.timestamp,ctime(&curtime),24);
                          tempmsg.source_id = 2;
			  tempmsg.log_level = 0;
                          tempmsg.data = 1;
			     mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
                        //memcpy(,buffer, strlen(buffer));
		      while(1)
                {
                        time(&curtime);
                        float temp = read_temp_data_reg(0);
                        if(temp == -300)
                        {
                          time(&curtime);
                          memcpy(tempmsg.random_string,"Error in getting data from Temperature task",strlen("Error in getting data from Temperature task"));
                          memcpy(tempmsg.timestamp,ctime(&curtime),24);
                          tempmsg.source_id = 2;
                          tempmsg.data = 1;
			  tempmsg.log_level = 2;
                          mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
                           
                        }
                        else
                        {
			                   memcpy(tempmsg.random_string,"Temperature data obtained",25);
			                   memcpy(tempmsg.timestamp,ctime(&curtime),24);
                         tempmsg.source_id = 2;
			 tempmsg.log_level = 1;
                         tempmsg.data = 1;
			                   mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);

                          //printf("The current temp is %f\n", temp);
                          //tempmsg.source_id = 2;
                          tempmsg.data = 0;
                          tempmsg.value = temp;
                          time(&curtime);
                          memcpy(tempmsg.timestamp,ctime(&curtime),24); 
                          if( mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1)== -1)
                          {
                            printf("Sending failed\n");
                          }
                          temp_client();
                        } 
                        sleep(1);

                }
		  }
      else
	   {
        printf("[Temperature Thread] Error initialising temperature task\n");
        time(&curtime);
        memcpy(tempmsg.random_string,"Error in initialising temperature task",strlen("Error in initialising temperature task"));
        memcpy(tempmsg.timestamp,ctime(&curtime),24);
        tempmsg.source_id = 2;
        tempmsg.data = 1;
	tempmsg.log_level = 2;
	       mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
         exit(0);
	   }

        time(&curtime);
        memcpy(tempmsg.random_string,"Temperature task finished\n",25);
        memcpy(tempmsg.timestamp,ctime(&curtime),24);
        tempmsg.source_id = 2;
	tempmsg.log_level = 1;
        tempmsg.data = 1;
	       mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
        
        printf("[Temperature Thread] Temperature Thread Finished\n");

}

void* logger_task()
{       
        FILE *fptr;
        mqd_t my_queue; 

        fptr = fopen(file_name,"w");   //use logger_thread -> filename
        my_queue = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
        struct mq_attr *pact;
        pact = malloc(sizeof(struct mq_attr));
        mq_getattr(my_queue,pact);
        //fprintf(fptr,"Message queue initialised\n");
        printf("[Logger Thread] Message queue initialised.\n");
        fclose(fptr);
while(1)
{
        fptr = fopen(file_name,"a");
        //my_queue = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
        mystruct given;
        //struct mq_attr *pact;
        //pact = malloc(sizeof(struct mq_attr));
        //mq_getattr(my_queue,pact);

        mq_receive(my_queue,(char *)&given,pact->mq_msgsize,NULL);

        char buffer[25]={0}, stringbuffer[50] = {0} ;
        memcpy(buffer, given.timestamp, 24);
        memcpy(stringbuffer, given.random_string, strlen(given.random_string));

        if(given.source_id == 0)  //for main task
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d, Message from main task: %s\n",buffer,given.source_id,given.log_level,stringbuffer);


        }

        else if(given.source_id == 1) //for light task
        {
        if(given.data == 0)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d,Lux value: %f\n",buffer,given.source_id,given.log_level,given.value);

        }
        else if(given.data == 1)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d, Message from light task: %s\n",buffer,given.source_id,given.log_level,stringbuffer);

        }       
        }

        else if(given.source_id == 2)  //for temp task
        {
        if(given.data == 0)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d, Temperature value: %f\n",buffer,given.source_id,given.log_level,given.value);

        }
        else if(given.data == 1)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d, Message from temp task: %s\n",buffer,given.source_id,given.log_level,stringbuffer);

        }       
        }
	
	else if(given.source_id == 3)  //for socket task
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Log ID:%d, Message from socket task: %s\n",buffer,given.source_id,given.log_level,stringbuffer);

       
        }      
        fclose(fptr);

}
        printf("[Logger Thread] Terminating message queue\n");
        return fptr;
}


void* func_socket()
{
  printf("[Socket Thread] Socket Task Started\n");
   time_t curtime;
   time(&curtime);
   mqd_t mq1;
  mystruct sample;
   //char buffer1[50] = {0};
   char buffer[50] = {0};
   //char my_stamp[25]; 

   //strncpy(buffer1,,27);
   strncpy(sample.random_string,"Socket task initiated", strlen("Socket task initiated"));
   sample.source_id = 3;
   sample.log_level = 0;
   //my_stamp = ctime(&curtime);
   memcpy(buffer,ctime(&curtime),24);
   memcpy(sample.timestamp,buffer, strlen(buffer));  
   
   mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
   
   if( mq_send(mq1,(char *)&sample,sizeof(sample),1)== -1)
        {
          printf("Sending failed\n");
        } 
    
  socket_task();
  printf("[Socket Thread] Socket Task Finished\n");
}

int check_status()
{
  struct sockaddr_in addr, peer_addr;
  int addr_len = sizeof(peer_addr);
  char rdbuff[1024] = {0};
  int server_socket, accepted_soc, opt = 1;
  int i = 0;
  payload_t *ploadptr;
  int read_b;
  size_t pload_len = 0;

  /* create socket */
  if((server_socket = socket(AF_INET,SOCK_STREAM,0)) == 0)
  {
    printf("[HBServer] [ERROR] Socket Creation Error\n");
    return 1;
  }
  else
    printf("[HBServer] Socket Created Successfully\n");

  /* set socket options */
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(opt), sizeof(opt)))
  {
    printf("[HBServer] [ERROR] Socket options set error\n");
    return 1;
  }

  /*Set the sockaddr_in structure */
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;  
  addr.sin_port = htons(HB_PORT_ADR);

  /*bind socket to a address */
  if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
  {
    printf("[HBServer] [ERROR] Bind socket Error\n");
    return 1;
  }
  else
    printf("[HBServer] Socket binded Successfully\n");

  /* listen for connections*/
  if(listen(server_socket,5) < 0)
  {
    printf("[HBServer] [ERROR] Can't listen connection\n");
    return 1;
  }
while(1)
{
  /*accept connection */
  accepted_soc = accept(server_socket, (struct sockaddr*)&peer_addr,(socklen_t*)&addr_len);
  if(accepted_soc < 0)
  {
    printf("[HBServer] [ERROR] Can't accept connection\n");
    return 1;
  }

  // read payload length 
  read_b = read(accepted_soc, &pload_len, sizeof(size_t));
  if(read_b == sizeof(size_t))
  {
    //printf("[HBServer] Size of incoming payload: %d\n",pload_len);
  } 
  else
  {
    //printf("[HBServer] [ERROR] Invalid data\n");
    return 1;
  } 

  // read payload 
  while((read_b = read(accepted_soc, rdbuff+i, 1024)) < pload_len)
  {
    i+=read_b;  
  }
  ploadptr= (payload_t*)rdbuff;
  /* display data */
  printf("[HBServer]  Message: %s\n",ploadptr->buf);
  
  // send message from server to client 
  //send(accepted_soc , "ACK" , 4, 0);
  //printf("[HBServer] Message sent from Server: ACK\n");
}
  /*close socket */
  close(accepted_soc);

  return 0;
}


int startup_test()
{
	int x=1;
	
	if(temp_init() == -1)
		x=0;

	if(light_init() == -1)
		x=0;

	if(pthread_create(&light_id, NULL,func_light,NULL) != 0)
		x=0;
   	if(pthread_create(&temp_id, NULL,func_temp,NULL) != 0)
   		x=0;
   	
   	if(pthread_create(&socket_id, NULL, func_socket, NULL ) !=0)
   		x=0;

	return x;
}

int main(int argc, char *argv[])
{
   
	memset(file_name, '\0', sizeof(file_name));
	strncpy(file_name, argv[1], strlen(argv[1]));	

   time_t curtime;
   time(&curtime);
   mqd_t mq1;
   mystruct sample;
   char buffer[50] = {0};
 
   pthread_create(&logger_id, NULL,logger_task,NULL);
   //pthread_create(&light_id, NULL,func_light,NULL);
   //pthread_create(&temp_id, NULL,func_temp,NULL);
   //pthread_create(&socket_id, NULL, func_socket, NULL );
   //thread1 -> filename = "log.txt";

	int startup_check = startup_test();

	if(startup_check == 0)
	{
		strncpy(sample.random_string,"Startup test failed", strlen("Startup test failed"));
   sample.source_id = 0;
   sample.log_level = 2;
   //my_stamp = ctime(&curtime);
   memcpy(buffer,ctime(&curtime),24);
   memcpy(sample.timestamp,buffer, strlen(buffer));
   mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
   mq_send(mq1,(char *)&sample,sizeof(sample),1);

		printf("\n<<<Startup Test Failed>>>\n\n");
		printf("[Main Task] Killing All Tasks\n");
		pthread_cancel(logger_id);
		pthread_cancel(temp_id);
		pthread_cancel(light_id);
		pthread_cancel(socket_id);	
		mq_close(mq1);
   		mq_unlink("/my_queue");
		return -1;
	}

   
   //char buffer1[50] = {0};
   //char buffer[50] = {0};
   //char my_stamp[25]; 

   //strncpy(buffer1,,27);
   strncpy(sample.random_string,"Initiated child threads", strlen("Initiated child threads"));
   sample.source_id = 0;
   sample.log_level = 0;
   //my_stamp = ctime(&curtime);
   memcpy(buffer,ctime(&curtime),24);
   memcpy(sample.timestamp,buffer, strlen(buffer));  
   
   mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
   if(mq1 == -1)
   {
      printf("Can't open\n"); //opening queue 1
      return -1;
    }
   
   if( mq_send(mq1,(char *)&sample,sizeof(sample),1)== -1)
        {
          printf("Sending failed\n");
        } 
    //mq_close(mq1);
   //mq_unlink("/my_queue");
   




   check_status();

   pthread_join(logger_id,NULL);
   pthread_join(light_id,NULL);
   pthread_join(temp_id,NULL);
   pthread_join(socket_id,NULL);
   pthread_exit(NULL);

   printf("Main Process Terminated\n");
  return 0;
}




