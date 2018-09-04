/*===============  Gpio.h ===============*/
/*
BY:	Aravind Dhulipalla
		Microprocessors Systems II and Embedded Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
Purpose:
Interface to the GPIO.cpp file

Date : 3rd Dec 2017
*/

#ifndef GPIO_H
#define GPIO_H

//----- f u n c t i o n    p r o t o t y p e s -----
void Export();
void UnExport();
void SetGPIO_output();
void SetGPIO_Input();
int read_gpio();
int StrtoInt(char data);

#endif