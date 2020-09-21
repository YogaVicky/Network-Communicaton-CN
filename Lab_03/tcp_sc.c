#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
// Function to calculate Time it takes to receive data completely
int duration(struct timeval *start, struct timeval *stop, struct timeval *delta)
{
	suseconds_t microstart, microstop, microdelta;

	microstart = (suseconds_t)(100000 * (start->tv_sec)) + start->tv_usec;
	microstop = (suseconds_t)(100000 * (stop->tv_sec)) + stop->tv_usec;
	microdelta = microstop - microstart;

	delta->tv_usec = microdelta % 100000;
	delta->tv_sec = (time_t)(microdelta / 100000);

	if ((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
		return -1;
	else
		return 0;
}

void interact(int c_sock, socklen_t add) //Function that receives data and creates a copy of the file
{
	char buf[3];
	char filename[100];
	char string[100];
	strcpy(string, "copy_");

	struct timeval start, stop, delta;
	FILE *fp;

	recv(c_sock, filename, sizeof(filename), 0);

	if (strcmp(filename, "EXIT") == 0)
	{
		printf("File Does not exist\n");
		exit(0);
	}

	strcat(string, filename);
	printf("%s", string);

	fp = fopen(string, "w");
	if (fp == NULL)
	{
		printf("Error opening file\n");
		exit(0);
	}

	gettimeofday(&start, NULL);
	bzero(buf, 3);
	recv(c_sock, buf, sizeof(buf), 0);

	while (strcmp(buf, "-1") != 0)
	{
		fprintf(fp, "%c", buf[0]);
		bzero(buf, 3);
		recv(c_sock, buf, sizeof(buf), 0);
	}

	gettimeofday(&stop, NULL);
	duration(&start, &stop, &delta);

	printf("\nTotal Time taken to receive(LATENCY): %ld.%ld \n", abs(delta.tv_sec), abs(delta.tv_usec));

	printf("File received\n");
	fclose(fp);
}

int main()
{

	int s_sock, c_sock, len;

	//socket file descriptor
	s_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (s_sock > 0)
	{

		struct sockaddr_in server_addr, client_addr;

		memset(&server_addr, 0, sizeof(server_addr));
		memset(&client_addr, 0, sizeof(client_addr));

		//server information
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(9046);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		socklen_t add;
		add = sizeof(client_addr);

		if (bind(s_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) >= 0)
		{
			printf("Server Binded\n");
			if ((listen(s_sock, 10)) != -1)
			{
				printf("Server Listening for connection\n");
				len = sizeof(client_addr);
				c_sock = accept(s_sock, (struct sockaddr *)&client_addr, &len);

				if (c_sock != -1)
				{
					//main prog replace
					printf("Connection Accepted\n");
					interact(c_sock, add);
					//main progra replace
				}

				else
				{
					printf("ERROR FOUND AT ACCEPT:\n");
				}

			} //listen
			else
			{
				printf("ERROR FOUND AT LISTEN:\n");
			}
		}

		else
		{
			printf("ERROR FOUND AT BIND:\n");
		}
	}
	else
	{

		printf("ERROR FOUND AT SOCKET:\n");
	}

	close(s_sock);
	close(c_sock);
	return 0;
}
