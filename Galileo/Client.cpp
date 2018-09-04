/*=============== Client.cpp ===============
BY:	Aravind Dhulipalla
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell

PURPOSE
Client thread which imlements a server connection and sends dat using CURL library

*/

#include "Client.h"
#include "Globals.h"
#include <sys/stat.h>
#include <time.h>

/*--------------- Client( ) ---------------*/
/*
Purpose
Interface to the pthread client
To send the captured image and sensor data to the Server using Curl library

Input Parameters 
threadid
*/

void *Client(void *clientid)
{
	while(1)
	{
		if(capture ==1) //only run if there is an image that is captured
		{
			printf("sending pic value\n");
			const char* hostname="ec2-54-202-113-131.us-west-2.compute.amazonaws.com"; // Server Hostname or IP address
			const int   port=8000;                  // Server Service Port Number
			const int   id=12;
			const char* password="password";
			const char* name="Aravind,Zubair";
			const int   adcval=ldrvalue;
			const char* status="HelloAll";
			const char* timestamp=time_stamp();
			char* filename="img.jpg";  // captured picture name

			char buf[1024];	
     
			//store the server url protocol to the buffer
			//............
			//provided URL Protocol in the lab description: replace the "server_hostname", "portnumber", "var_xxxx" with the related format specifiers "%d" or "%s" 
			sprintf(buf,"http://%s:%d/update?id=%d&password=%s&name=%s&data=%d&status=%s&timestamp=%s&filename=%s",
			hostname,
			port,
			id,
			password,
			name,
			adcval,
			status,
			timestamp,
			filename);
	
			
	
			FILE *fp; 
			struct stat num;
			stat(filename, &num);
			int size = num.st_size;
			char *buffer = (char*)malloc(size);

			HTTP_POST(buf, buffer, size);	
			fclose(fp);
			pthread_mutex_lock(&mutex2); //lock the mutex 
			capture = 0; //change the capture value
			pthread_mutex_unlock(&mutex2); //unlock the mutex
		}
	}
}

/*--------------- HTTP_POST( ) ---------------*/
/*
Purpose
To send the data to server using http protocol from CURL library

Input Parameters 
url - Address of the URL
image - Address of the image
size - size of the packet
*/

void HTTP_POST(const char* url, const char* image, int size)
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,(long) size);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, image);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
      			fprintf(stderr, "curl_easy_perform() failed: %s\n",
              			curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
}

/*--------------- time_stamp( ) ---------------*/
/*
Purpose
To get the appropriate local time from galileo

Returns 
address of the timestamp
*/

char *time_stamp()
{
char *timestamp = (char *)malloc(sizeof(char) * 16); 
time_t ltime; //
ltime=time(NULL); 
struct tm *tm;
tm=localtime(&ltime); //Using the linux localtime function

sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon+1, 
    tm->tm_mday, tm->tm_hour-5, tm->tm_min, tm->tm_sec);
return timestamp;
}