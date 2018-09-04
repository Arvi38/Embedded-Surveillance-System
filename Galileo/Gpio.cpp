/*===============  Gpio.cpp ===============*/
/*
BY:	Aravind Dhulipalla
		Microprocessors Systems II and Embedded Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
Purpose:
Contains the functions to manipulate GPIO pins

Date : 3rd Dec 2017
*/

#include "Gpio.h"


/*--------------- Export ( ) ---------------*/

/*
PURPOSE
Exports the required GPIO pins.
*/
void Export()
{
	//export the pin 8 GPIO 40
		system("echo 40 > /sys/class/gpio/export");
		//export the pin 7 GPIO 38
        system("echo 38 > /sys/class/gpio/export");
		//export pin 6 GPIO 1 and SHIFTER GPIO 20
		system("echo 1 > /sys/class/gpio/export");
		system("echo 20 > /sys/class/gpio/export");
		//export pin 5 GPIO 0 and SHIFTER GPIO 18
		system("echo 0 > /sys/class/gpio/export");
		system("echo 18 > /sys/class/gpio/export");
		//export pin 4 GPIO 6 and SHIFTER GPIO 36
		system("echo 6 > /sys/class/gpio/export");
		system("echo 36 > /sys/class/gpio/export");
}

/*--------------- UnExport ( ) ---------------*/

/*
PURPOSE
UnExports the required GPIO pins.
*/
void UnExport()
{
	//export the pin 8 GPIO 40
		system("echo 40 > /sys/class/gpio/unexport");
		//export the pin 7 GPIO 38
        system("echo 38 > /sys/class/gpio/unexport");
		//export pin 6 GPIO 1 and SHIFTER GPIO 20
		system("echo 1 > /sys/class/gpio/unexport");
		system("echo 20 > /sys/class/gpio/unexport");
		//export pin 5 GPIO 0 and SHIFTER GPIO 18
		system("echo 0 > /sys/class/gpio/unexport");
		system("echo 18 > /sys/class/gpio/unexport");
		//export pin 4 GPIO 6 and SHIFTER GPIO 36
		system("echo 6 > /sys/class/gpio/unexport");
		system("echo 36 > /sys/class/gpio/unexport");
}

/*--------------- SetGPIO_output ( ) ---------------*/

/*
PURPOSE
Sets the requried GPIO pins as output.
*/
void SetGPIO_output()
{
     //setting pin8 as an output   
	system("echo out > /sys/class/gpio/gpio40/direction");
	//Setting pin7 as an output
	system("echo out > /sys/class/gpio/gpio38/direction");
	//setting pin6 as an output
	system("echo out > /sys/class/gpio/gpio1/direction");
	system("echo out > /sys/class/gpio/gpio20/direction");
	//setting pin5 as an output
	system("echo out > /sys/class/gpio/gpio0/direction");
	system("echo out > /sys/class/gpio/gpio18/direction");
	//setting pin4 as output
	system("echo out > /sys/class/gpio/gpio6/direction");
	system("echo out > /sys/class/gpio/gpio36/direction");         
}

/*--------------- SetGPIO_Input( ) ---------------*/

/*
PURPOSE
Exports the required GPIO pins.
*/
void SetGPIO_Input()
{
	//Setting pin7 as an input
	system("echo in > /sys/class/gpio/gpio38/direction");
	//setting pin6 as an input
	system("echo in > /sys/class/gpio/gpio1/direction");
	system("echo in > /sys/class/gpio/gpio20/direction");
	//setting pin5 as an input
	system("echo in > /sys/class/gpio/gpio0/direction");
	system("echo in > /sys/class/gpio/gpio18/direction");
	//setting pin4 as input
	system("echo in > /sys/class/gpio/gpio6/direction");
	system("echo in > /sys/class/gpio/gpio36/direction");  
}

/*--------------- StrtoInt( ) ---------------*/

/*
PURPOSE
Convert the data character to int.

Returns
integer value either 1 or 0;
*/
int StrtoInt(char data)
{
	int value;
	if(data == '0')
		value =0;
	if(data == '1')
		value = 1;
	
	return value;
}

/*--------------- read_gpio( ) ---------------*/

/*
PURPOSE
Exports the required GPIO pins.

Returns
The 10 bit ADC value received from the PIC
*/

int read_gpio()
{
	int a;
    FILE *fp;
	system("./gpio_in.sh 6");
    fp = fopen("out.txt","r");
    a = StrtoInt(fgetc(fp));
    fclose(fp);		
	system("./gpio_in.sh 0");
	fp = fopen("out.txt","r");
    a = a | (StrtoInt(fgetc(fp)) << 1);
    fclose(fp);
	system("./gpio_in.sh 1");
    fp = fopen("out.txt","r");
    a = a | (StrtoInt(fgetc(fp))<< 2);
    fclose(fp);		
	system("./gpio_in.sh 38");
	fp = fopen("out.txt","r");
    a = a| (StrtoInt(fgetc(fp))<<3);
    fclose(fp);
		
	return a;
}