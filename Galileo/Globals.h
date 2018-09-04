/*===============  Globals.cpp ===============*/

/*
BY:	Aravind Dhulipalla
		Microprocessors Systems II and Embedded Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
Purpose:
Contains the Global definitions used in the project

Date : 3rd Dec 2017

*/

#ifndef GLOBALS_H
#define GLOBALS_H

extern pthread_mutex_t mutex1,mutex2; //mutex variables to protext shared variables
extern int update; //Update shared variable to indicate new update from user
extern int capture; //capture shared variable to indicate image capture
extern int ldrvalue;

#endif