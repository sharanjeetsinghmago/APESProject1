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
#include "light_task.h"
#include "light_task.c"
#include "temp_task.h"
#include "temp_task.c"


typedef struct              //structure to be sent
{             
char timestamp[50];     
int source_id;
//int Log_level;
int data;
float value;
char random_string[50];
}mystruct;

struct threadParam
{
char *filename;
};

void *func_light()
{       mqd_t mq1;
        printf("Light Thread Started\n");
        mystruct lightmsg;
        time_t curtime;
        int x = light_init();
        if(x != -1)
        {
                mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
                time(&curtime); 
                memcpy(lightmsg.timestamp,ctime(&curtime), strlen(ctime(&curtime)));
                memcpy(lightmsg.random_string,"Light task initiated",20);

                lightmsg.source_id = 1;
                lightmsg.data =1;
                mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);      
	while(1)
        {       
                //time_t curtime;
                time(&curtime);
                float lumen = get_lux(x);
                
                if(lumen < 0 )
		{
 		time(&curtime);
          	memcpy(lightmsg.random_string,"Error in getting data from light task",strlen("Error in getting data from light task"));
       		memcpy(lightmsg.timestamp,ctime(&curtime),24);
        	lightmsg.source_id = 1;
        	lightmsg.data = 1;
		mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);
                sleep(1);
 		}
                else{    
		memcpy(lightmsg.random_string,"Light data obtained",19);
		memcpy(lightmsg.timestamp,ctime(&curtime),24);
                lightmsg.source_id = 1;
                lightmsg.data = 1;
		mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);

                printf("The current lux is %f\n", lumen);
                memcpy(lightmsg.timestamp,ctime(&curtime), strlen(ctime(&curtime)));
                lightmsg.data = 0;
                lightmsg.value = lumen; 
                if( mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1)== -1)
                 {
          printf("Sending failed\n");
                 } 
          sleep(1);
        }
        }
	}
        else 
        {
        time(&curtime);
        memcpy(lightmsg.random_string,"Error in initialising light task",strlen("Error in initialising light task"));
        memcpy(lightmsg.timestamp,ctime(&curtime),24);
        lightmsg.source_id = 2;
        lightmsg.data = 1;
	mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);
        }


        printf("Light Thread Finished\n");
	time(&curtime);
        memcpy(lightmsg.random_string,"Light task finished",19);
        memcpy(lightmsg.timestamp,ctime(&curtime),24);
        lightmsg.source_id = 2;
        lightmsg.data = 1;
	mq_send(mq1,(char *)&lightmsg,sizeof(lightmsg),1);
}



void *func_temp()
{
        mqd_t mq1;
        printf("Temperature Thread Started\n");
        mystruct tempmsg;
        time_t curtime;
        char buffer[50] = {0};
        if(temp_init() != -1)
		{
                //get_temp();
                        mq1 = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
                        memcpy(tempmsg.random_string,"Temperature task initiated",26);
                        time(&curtime);
                        memcpy(tempmsg.timestamp,ctime(&curtime),24);
                        tempmsg.source_id = 2;
                        tempmsg.data = 1;
			mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
                        //memcpy(,buffer, strlen(buffer));
		while(1)
                {
                        time(&curtime);
                        float temp = read_temp_data_reg();
			memcpy(tempmsg.random_string,"Temperature data obtained",25);
			memcpy(tempmsg.timestamp,ctime(&curtime),24);
                        tempmsg.source_id = 2;
                        tempmsg.data = 1;
			mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);

                        printf("The current temp is %f\n", temp);
                        //tempmsg.source_id = 2;
                        tempmsg.data = 0;
                        tempmsg.value = temp;
                        time(&curtime);
                        memcpy(tempmsg.timestamp,ctime(&curtime),24); 
                        if( mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1)== -1)
                        {
                        printf("Sending failed\n");
                        } 
                        sleep(1);

                }
		}
        else
	{
        printf("Error initialising temperature task");
        time(&curtime);
        memcpy(tempmsg.random_string,"Error in initialising temperature task",strlen("Error in initialising temperature task"));
        memcpy(tempmsg.timestamp,ctime(&curtime),24);
        tempmsg.source_id = 2;
        tempmsg.data = 1;
	mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
	}

        time(&curtime);
        memcpy(tempmsg.random_string,"Temperature task finished",25);
        memcpy(tempmsg.timestamp,ctime(&curtime),24);
        tempmsg.source_id = 2;
        tempmsg.data = 1;
	mq_send(mq1,(char *)&tempmsg,sizeof(tempmsg),1);
        
        printf("Temperature Thread Finished\n");

}

void* logger_task(void *arg)
{       
        struct threadParam *logger_thread = (struct threadParam*) arg;
        
        FILE *fptr;
        mqd_t my_queue; 

        fptr = fopen("log.txt","w");   //use logger_thread -> filename
        my_queue = mq_open("/my_queue",O_RDWR | O_CREAT, 0666, NULL);
        struct mq_attr *pact;
        pact = malloc(sizeof(struct mq_attr));
        mq_getattr(my_queue,pact);
        fprintf(fptr,"Message queue initialised\n");
        printf("Message queue initialised.\n");
        fclose(fptr);
while(1)
{
        fptr = fopen("log.txt","a");
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

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Message from main task: %s\n",buffer,given.source_id,stringbuffer);


        }

        else if(given.source_id == 1) //for light task
        {
        if(given.data == 0)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Lux value: %f\n",buffer,given.source_id,given.value);

        }
        else if(given.data == 1)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Message from light task: %s\n",buffer,given.source_id,stringbuffer);

        }       
        }

        else if(given.source_id == 2)  //for temp task
        {
        if(given.data == 0)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Temperature value: %f\n",buffer,given.source_id,given.value);

        }
        else if(given.data == 1)
        {

        fprintf(fptr,"Timestamp:%s, Source ID:%d, Message from temp task: %s\n",buffer,given.source_id,stringbuffer);

        }       
        }
	       //mq_close(my_queue);
        //mq_unlink("/my_queue");
        fclose(fptr);

}
        printf("Terminating message queue\n");
        return fptr;
}

int main()
{
   pthread_t id1; 
   pthread_t id2;
   pthread_t id3;
   time_t curtime;
   time(&curtime);
   mqd_t mq1;

   struct threadParam* thread1 =
           (struct threadParam*) malloc(sizeof(struct threadParam));
   struct threadParam* thread2 =
           (struct threadParam*) malloc(sizeof(struct threadParam));
   struct threadParam* thread3 =
           (struct threadParam*) malloc(sizeof(struct threadParam));
   pthread_create(&id1,NULL,logger_task,(void *)thread1);
   pthread_create(&id2,NULL,func_light,(void *)thread2);
   pthread_create(&id3,NULL,func_temp,(void *)thread3);
   //thread1 -> filename = "log.txt";

   mystruct sample;
   char buffer1[50] = {0};
   char buffer[50] = {0};
   //char my_stamp[25]; 

   //strncpy(buffer1,,27);
   strncpy(sample.random_string,"Initiated child threads", strlen("Initiated child threads"));
   sample.source_id = 0;

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
   printf("Terminating process 1\n");

   pthread_join(id1,NULL);
   pthread_join(id2,NULL);
   pthread_join(id3,NULL);
   pthread_exit(NULL);
  return 0;
}




