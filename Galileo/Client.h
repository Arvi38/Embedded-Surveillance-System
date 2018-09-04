/*=============== Gpio . h ===============
BY:	Aravind Dhulipalla
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell

PURPOSE
Inteface to Client.cpp

*/

#ifndef Client_H
#define Client_H

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void *Client(void *clientid);
void HTTP_POST(const char* url, const char* image, int size);
char *time_stamp();