/*===============  Lab4. cpp ===============*/

/*
BY:	Aravind Dhulipalla
		Microprocessors Systems II and Embedded Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
Purpose:
The main purpose of this lab is to understand the multithreading programing using Pthreads. 
Synchronization of those threads using Mutex. 
Understanding usage of curl library, HTTP protocol using a client and server application. 
Understanding of image processing using OpenCV library.

Date : 3rd Dec 2017

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <time.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "Gpio.h"
#include "Globals.h"
#include "Client.h"
#include "interface.h"

using namespace cv;
using namespace std;

//----- c o n s t a n t    d e f i n i t  i o n s -----

	#define UP 1 //UP gesture
	#define DOWN 2 //Down gesture
	#define LEFT 3  //Left gesture
	#define RIGHT 4 //right gesture
	#define ACK 0xF //Acknowledge
	#define tenmsec 10000
	
//----- g l o b a l    v a r i a b l e s -----

	pthread_mutex_t mutex1,mutex2; //mutex variables to protext shared variables
	int ldrvalue; //to store ldrvalue
	int update; //update shared variable between threads
	int capture=0; //to indicate a photo capture by camera 
	int cmd; //
	char buffer[100]; //buffer for HTTP requests

	useconds_t delay = 2000;  //delay of 2sec
	char *dev = "/dev/i2c-0"; //i2C device node
 	int  fd = open(dev, O_RDWR ); //open the i2C device file for read and write
	

	  
/*--------------- APDS9960_write( ) ---------------*/
/*
Purpose
To send the command over the i2C bus

Input Parameters 
address - Address of the i2C device
command - Read/ Write command

Returns
True - If if the command is sent
False - If the bus is busy/ command failed
*/

bool APDS9960_write(unsigned char address,unsigned char command)
	{
		unsigned char command1[2] = {address,command};
		int r = write(fd,&command1,2);
		if(r<0)
		{
			printf("error wrinting to address: %d",address);
			return false;
		}
		else
		return true;
	}

/*--------------- Imagecapture( ) ---------------*/
/*
Purpose
To capture the image using the openCV library and save to a file
*/

void Imagecapture()
	{
		VideoCapture cap(0); // open the video camera no. 0

		if (!cap.isOpened())  // if not success, exit program
		{
			cout << "ERROR: Cannot open the video file" << endl;
			
		}
	   double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	   double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	   cout << "Frame Size = " << dWidth << "x" << dHeight << endl;
	   vector<int> compression_params; //vector that stores the compression parameters of the image
	   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	   compression_params.push_back(95); //specify the jpeg quality
	   Mat img(dWidth, dHeight, CV_8UC1);
	   cap.read(img);
	   static int i =0;
	   snprintf(buffer,100,"Img%d.jpg",i);
	   i++;
	   bool bSuccess = imwrite(buffer, img, compression_params); //write the image to file
	   if ( !bSuccess )
	   {
			cout << "ERROR : Failed to save the image" << endl;
	   }
	}
	
/*--------------- read_gesture( ) ---------------*/
/*
Purpose 
Reads the gesture value from the APDS9960 and sense the gesture.

Returns
Gesture Value as in UP,DOWN,LEFT or RIGHT
*/
unsigned char read_gesture()
	{
		unsigned char  GF4 = 0xAB;
		unsigned char  STATUS = 0x93;
		unsigned char  GFLVL = 0xAE;
		unsigned char  GSTATUS = 0xAF;
	    unsigned char  GUP = 0xFC;
		unsigned char  GDOWN = 0xFD;
		unsigned char  GLEFT = 0xFE;
	    unsigned char  GRIGHT = 0xFF;
		unsigned char  GF4_V,STATUS_V,GFLVL_V,GSTATUS_V;
		unsigned char  GUP_V[32] , GDOWN_V[32], GLEFT_V[32] ,GRIGHT_V[32] ;
		unsigned char  valid_up[1],valid_down[1],valid_left[1],valid_right[1];
		while(1)
		{
			write(fd,&GF4,1);
			usleep(delay);
			read(fd,&GF4_V,1);
			//printf("Status : %d\n",GF4_V);
			write(fd,&STATUS,1);
			usleep(delay);
			read(fd,&STATUS_V,1);
			//printf("Status : %d\n",STATUS_V);
		    write(fd,&GSTATUS,1);
		    usleep(delay);
		    read(fd,&GSTATUS_V,1);
		    //printf("GSTATUS: %d\n",GSTATUS_V);
			unsigned char x = GSTATUS_V & 0x01;
			//printf("x = %d",x);
			unsigned char y = STATUS_V & 0x02;
			//printf("y = %d",y);
			if(((GSTATUS_V & 0x01) ==1) && ((STATUS_V & 0x04) == 4 ))
			{
				if(!APDS9960_write(0xAB,0x03))
				{
					return false;
				}
				sleep(1);
				//printf("valid\n");
				write(fd,&GFLVL,1);
				usleep(delay);
				read(fd,&GFLVL_V,1);
				//printf("GFLVL: %d\n",GFLVL_V);			
				for(int i=0;i<=GFLVL_V-1;i++) // for reading the 32 datasets
				{		 
					sleep(0.7);
					write(fd,&GUP,1);
					usleep(delay);
					read(fd,&GUP_V[i],1);		 
					//printf("GUP: %d\n",GUP_V[i]);
					write(fd,&GDOWN,1);
					usleep(delay);
					read(fd,&GDOWN_V[i],1);
					//printf("GDOWN: %d\n",GDOWN_V[i]);
					write(fd,&GLEFT,1);
					usleep(delay);
					read(fd,&GLEFT_V[i],1);
					//printf("GLEFT: %d\n",GLEFT_V[i]);
					write(fd,&GRIGHT,1);
					usleep(delay);
					read(fd,&GRIGHT_V[i],1);
					//printf("GRIGHT: %d\n",GRIGHT_V[i]);
				}
				if(!APDS9960_write(0xAB,0x00))
				{
					return false;
				}
				valid_up[1]={0};
				valid_down[1] = {0};
				valid_left[1] = {0};
				valid_right[1] = {0};
				for(int j=0;j<GFLVL_V-1;j++)
				{
					if(GUP_V[j] >50){valid_up[0] = GUP_V[j];}
					if(GDOWN_V[j] >50){valid_down[0] = GDOWN_V[j];}
					if(GLEFT_V[j] >50){valid_left[0] = GLEFT_V[j];}
					if(GRIGHT_V[j] >50){valid_right[0] = GRIGHT_V[j];}			
				}
				if((valid_up[0] == valid_down[0]) && (valid_left[0] == valid_right[0]) && (valid_down[0] == valid_left[0]))
				{cout << "Give a Gesture please"<<endl;}
				if((valid_down[0] < valid_up[0]) && (valid_left[0] > valid_right[0]))
				{
					cout << "UP GESTURE DETECTED" << endl;
					return UP;         
				}
				if((valid_down[0] > valid_up[0]) && (valid_left[0] > valid_right[0]))
				{
					cout << "Down GESTURE DETECTED" << endl;
					return DOWN;         
				}
				if((valid_down[0] > valid_up[0]) && (valid_left[0] < valid_right[0]))
				{
					cout << "Left GESTURE DETECTED" << endl;
					return LEFT;         
				}
				if((valid_down[0] < valid_up[0]) && (valid_left[0] < valid_right[0]))
				{
					cout << "Right GESTURE DETECTED" << endl;
					return RIGHT;         
				}
				else 
				{
					cout << "Wrong GESTURE DETECTED Please Try again" << endl;
					break;
				}
		 }
		 else
		 {
			// printf("not valid");
			 write(fd,&GFLVL,1);
			 usleep(delay);
			 read(fd,&GFLVL_V,1);
			 printf("GFLVL: %d\n",GFLVL_V);
			 for(int i=1;i<=GFLVL_V;i++)
			 {
					write(fd,&GUP,1);
					usleep(delay);
					read(fd,&GUP_V[i],1);		 
					write(fd,&GDOWN,1);
					usleep(delay);
					read(fd,&GDOWN_V[i],1);
					write(fd,&GLEFT,1);
					usleep(delay);
					read(fd,&GLEFT_V[i],1);
					write(fd,&GRIGHT,1);
					usleep(delay);
					read(fd,&GRIGHT_V[i],1);
			 }
			if(!APDS9960_write(0xAB,0x00))
			{
				return false;
			}
		} 	 
		}
		return 0;
	}
/*--------------- gesture_enable( ) ---------------*/
/*
Purpose
Configures the Gesture sensor to required register values

Returns 
True - if the configuration is done properly
False - if not configured
*/	
bool gesture_enable()
	{
		if(!APDS9960_write(0xA1,0x00))
		{
			return false;
		}
		//Config1
		if(!APDS9960_write(0xA2,0x00))
		{
			return false;
		}
		//Config2
		if(!APDS9960_write(0xA3,0x41))
		{
			return false;
		}
		//Up Offstet Register
		if(!APDS9960_write(0xA4,0x00))
		{
			return false;
		}
		//Down offset register
		if(!APDS9960_write(0xA5,0x00))
		{
			return false;
		}
		//Left offset register
		if(!APDS9960_write(0xA7,0x00))
		{
			return false;
		}
		//right offset register
		if(!APDS9960_write(0xA9,0x00))
		{
			return false;
		}
		//Pulse count length
		if(!APDS9960_write(0xA6,0x47))
		{
			return false;
		}
		//cofig3
		if(!APDS9960_write(0xAA,0x03))
		{
			return false;
		}
		//config 4
		if(!APDS9960_write(0xAB,0x03))
		{
			return false;
		}
		//clear interrupts
		if(!APDS9960_write(0xE7,0x00))
		{
			return false;
		}
			return true;

	}
/*--------------- Temperature( ) ---------------*/
/*Temperature()
Reads the temperature value from the sensor and returns the value.
------------------------------------------------------------------*/	
unsigned char Temperature()
	{
		int i;
		int r;
		int fd2;
		float result = 0.0;
		char value[2] ={0} ;
		char addr = 0x48;
		//const char *dev = "/dev/i2c-0";
		pthread_mutex_lock(&mutex1);
		fd = open(dev, O_RDWR );
		if(fd < 0)
		{
			perror("Opening i2c device node\n");
			return 1;
		}
		r = ioctl(fd, I2C_SLAVE, addr);
		if(r < 0)
		{
			perror("Selecting i2c device\n");
		}
		for(i=0;i<2;i++)
		{
            r = read(fd, &value[i], 1);
            if(r != 1)
            {
                perror("reading i2c device\n");
            }
            usleep(delay);
		}
		float  tlow =0;
		tlow = (float)(((value[0] << 8) | value[1]) >> 4);
		result = 0.0625*(tlow);
        printf("Temperature: %f\n",result);
        close(fd);
		pthread_mutex_unlock(&mutex1);
		return result;
}

/*--------------- Interface(void *Interfaceid) ---------------*/
/*
Purpose
read the input send by user on terminal window and provide the requested access.

Input:
Takes thread id and return nothing.
*/	

void *Interface(void *Interfaceid)
{
  int cmd,a,adc,data;
 
  while(1)
  {
	  char a = getchar(); //read the input given by user
	  if(a=='\n')
	  {
		printf("Enter pressed");
		pthread_mutex_lock(&mutex1);
		update = 1;
		pthread_mutex_unlock(&mutex1);
		printf("Give any one of the command \n 1.Reset 2.Ping 3.PIC LDR VALUE 4.TURN 30 5.TURN 90 6.TURN 120 7.Temperature\n");
		scanf("%d",&cmd);
		//make the strobe high
		switch(cmd)
		{
			case 1: // sending reset command(0x0) to pic controller
			Export(); //exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output(); //set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");// setting strobe signal to high
			system("echo 0 > /sys/class/gpio/gpio6/value");// setting data bit 0 to low
			system("echo 0 > /sys/class/gpio/gpio0/value");// setting data bit 1 to low
			system("echo 0 > /sys/class/gpio/gpio1/value");// setting data bit 2 to low
			system("echo 0 > /sys/class/gpio/gpio38/value");// setting data bit 3 to low
			usleep(tenmsec);//delaying for 10 msec to avoid errors while sending data
			system("echo 0 > /sys/class/gpio/gpio40/value");// setting strobe signal to low to give 10 msec strobe
			UnExport();//unexport the gpio pins
			Export();//exporting to read from gpio (data from pic)
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high
			a = read_gpio();	//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low to finish reading the data
			UnExport();
			if(a!=ACK)
			{
				printf("pic not available");
			}
			break;
			case 2:// sending ping command(0x1) to pic controller and wait for ack from pic
			Export();//exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output();//set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high
			system("echo 1 > /sys/class/gpio/gpio6/value"); //sending bit 0 of data as 1
			system("echo 0 > /sys/class/gpio/gpio0/value"); //sending bit 1 of data as 0
			system("echo 0 > /sys/class/gpio/gpio1/value"); //sending bit 2 of data as 0
			system("echo 0 > /sys/class/gpio/gpio38/value");//sending bit 3 of data as 0
			usleep(tenmsec);//setting 10 msec delay
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting the strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			Export();//exporting to read from gpio (data from pic)
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high
			a=read_gpio();//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low to finish reading the data
			UnExport();
			if(a!=ACK)//checking whether ack is received
			{
				printf("pic not available");
			}
			break;
			case 3:// sending ldr read command(0x2) to pic controller and wait ldr data from pic
			Export();//exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output();//set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high
			system("echo 0 > /sys/class/gpio/gpio6/value");//sending bit 0 of data as 0
			system("echo 1 > /sys/class/gpio/gpio0/value");//sending bit 1 of data as 1
			system("echo 0 > /sys/class/gpio/gpio1/value");//sending bit 2 of data as 0
			system("echo 0 > /sys/class/gpio/gpio38/value");//sending bit 3 of data as 0
			usleep(tenmsec);//setting 10 msec delay
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting the strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			Export();//exporting to read from gpio (data from pic)
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high
			a = read_gpio();//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low to finish reading the data
			if(a==ACK)//checking whether ack is received
			{
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high
			int data = read_gpio();//read the lower 4 bits of ldr value
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high to read data from pic
			data = data | (read_gpio()<<4);//read the higher 4 bits of ldr value
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting strobe signal high to read data from pic
			data = data | (read_gpio() << 8);//read the highest 4 bits of ldr value
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			pthread_mutex_lock(&mutex1);//locking this thread to avoid outside thread changing ldrvalue variable
			ldrvalue = data;//storing the ldr value to ldrvalue variable
			printf("%d\n",data);//printing the data on screen for user
			pthread_mutex_unlock(&mutex1);//unlocking the thread for other threads to access the ldrvalue variable
			}
			else
			{
				printf("pic not found");
				update = 0;//making other thread accessible
			}
			break; 
			case 4:// sending turn 30 command(0x3) to pic controller to control the angular turn by 30 degree of stepper motor connected to gpio of pic
					Export();//exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output();//set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high
			system("echo 1 > /sys/class/gpio/gpio6/value");//sending bit 0 of data as 1
			system("echo 1 > /sys/class/gpio/gpio0/value");//sending bit 1 of data as 1
			system("echo 0 > /sys/class/gpio/gpio1/value");//sending bit 2 of data as 0
			system("echo 0 > /sys/class/gpio/gpio38/value");//sending bit 3 of data as 0
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			Export();//exporting to read from gpio (data from pic)
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high to read data from pic
			a = read_gpio();//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			break;
			case 5:// sending turn 90 command(0x4) to pic controller to control the angular turn by 90 degree of stepper motor connected to gpio of pic
					Export();//exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output();//set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high
			system("echo 0 > /sys/class/gpio/gpio6/value");//sending bit 0 of data as 0
			system("echo 0 > /sys/class/gpio/gpio0/value");//sending bit 1 of data as 0
			system("echo 1 > /sys/class/gpio/gpio1/value");//sending bit 2 of data as 1
			system("echo 0 > /sys/class/gpio/gpio38/value");//sending bit 3 of data as 0
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			Export();//exporting to read ack from pic
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high to read data from pic
			a = read_gpio();//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			if(a!=ACK)//checking whether ack is received
			{
				printf("pic not ready");
			}
			break;
			case 6:// sending turn 120 command(0x5) to pic controller to control the angular turn by 120 degree of stepper motor connected to gpio of pic
					Export();//exporting the required gpio pins 40,6, 0, 1, 38 connected to pic, with 6,0,1&38 as data pins
			SetGPIO_output();//set the pins to output mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high
			system("echo 1 > /sys/class/gpio/gpio6/value");//sending bit 0 of data as 1
			system("echo 0 > /sys/class/gpio/gpio0/value");//sending bit 0 of data as 0
			system("echo 1 > /sys/class/gpio/gpio1/value");//sending bit 0 of data as 1
			system("echo 0 > /sys/class/gpio/gpio38/value");//sending bit 0 of data as 0
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			Export();//exporting to read ack from pic
			SetGPIO_Input();//setting the gpio(data pins) to input mode
			system("echo 1 > /sys/class/gpio/gpio40/value");//setting the strobe signal high to read data from pic
			a = read_gpio();//reading the data from pic while strobe is high
			usleep(tenmsec);
			system("echo 0 > /sys/class/gpio/gpio40/value");//setting strobe signal low
			UnExport();//unexporting for next reading/writing to gpio
			if(a!=ACK)//checking whether ack is received
			{
				printf("pic not ready");
			}
			break;
			
			case 7://reading temperature from temperature sensor
			unsigned char  Temp = Temperature();
			break;
		
		}
		sleep(2);
	  }
  }
}


/*--------------- Sensors( ) ---------------*/
/*
Purpose
Interface to the pthread client
To Collect the data from the PIC microcontroller to which sensors are attached

Input Parameters 
threadid
*/

void *Sensors(void *Sensorsid)
{
	while(1)
	{ 
        //pthread_mutex_lock(&mutex1); 
	    //cmd = update; 
	    //pthread_mutex_unlock(&mutex1);
		if(update == 0) //only run if there is an update
		{
	        unsigned char Temp_value = Temperature(); //get the temperature value
			pthread_mutex_lock(&mutex1); // lock mutex1
	        fd = open(dev, O_RDWR ); //open i2C device node
            int i,r; 
			int addr = 0x39; //address of the 
			if(fd < 0)
			{
					perror("\nOpening i2c device node\n");
			}
			r = ioctl(fd, I2C_SLAVE, addr);
			if(r < 0)
			{
					perror("\nSelecting i2c device\n");
			}
			gesture_enable();
			r = APDS9960_write(0x80,0x4D);
			if(r<0)
			{
					perror("\ngesture engine not started\n");
			}
			printf("\ngesture engine started\n");
			usleep(delay);
			unsigned char value = read_gesture();
			if(!APDS9960_write(0xAB,0x00))
			{
				printf("Error during write to sensor");
			}
			if(!APDS9960_write(0xE7,0x00))
			{
				printf("Error during write to sensor");
			}
			if(!APDS9960_write(0x80,0x00))
			{
				printf("Error during write to sensor");
			}
			if((Temp_value>20) ||(value == UP))
			{
				Imagecapture();
				cout<<"Gesture Recognised and Picture taken" << endl;
				pthread_mutex_lock(&mutex2);
				capture = 1;
				pthread_mutex_unlock(&mutex2);
			}
			else
			{
				cout <<"Gesture Not Correct or Recognised" << endl;
			}
			close(fd);
            pthread_mutex_unlock(&mutex1);	
		}			
	}
  	
}

/*--------------- main( ) ---------------*/
/*
Purpose
The main function for galileo
*/
int main(void)
{
	pthread_mutex_init(&mutex1,NULL); //Create a Mutex to protect i2C device node
	pthread_mutex_init(&mutex2,NULL); //Create a Mutex to protect capture variable
	pthread_t thread_client,thread_Interface,thread_Sensors; //variables for differet threads
	pthread_create(&thread_Interface,NULL,Interface,NULL); //Create Interface thread
	pthread_create(&thread_Sensors,NULL,Sensors,NULL); //Create Sensors thread
	sleep(0.01); //sleep for 1ms
	pthread_create(&thread_client,NULL,Client,NULL); //Create Client thread
	pthread_join(thread_Interface,NULL); //Wait for Interface thread to complete
	pthread_join(thread_Sensors,NULL); //Wait for Sensors thread to complete
	pthread_join(thread_client,NULL); //Wait for Client thread to complete
	pthread_mutex_destroy(&mutex1); //Destroy mutex1
	pthread_mutex_destroy(&mutex2); //Destroy mutex2
	return 0;
}