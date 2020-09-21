#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

void interact(int c_sock, socklen_t add) //Function that sends data and creates a copy of the file
{

	char buf[3], filename[100];

	FILE *fp;

	printf("Enter the filename along with extension to be copied\n");
	scanf("%s", filename);
	//send filename
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("File does not exist\n");
		strcpy(filename, "EXIT");
		send(c_sock, filename, sizeof(filename), 0);
		exit(0);
	}

	send(c_sock, filename, sizeof(filename), 0);

	char c;

	while (!feof(fp))
	{
		fscanf(fp, "%c", &buf[0]);

		send(c_sock, buf, sizeof(buf), 0);
	}
	bzero(buf, 3);

	strcpy(buf, "-1");
	send(c_sock, buf, sizeof(buf), 0);

	printf("File sent\n");
	fclose(fp);
}

int main()
{

	int c_sock;

	//Socket Descriptor
	c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock > 0)
	{

		struct sockaddr_in client;

		//Server info
		client.sin_family = AF_INET;
		client.sin_port = htons(22);
		client.sin_addr.s_addr = inet_addr("13.92.135.188");

		int ch = 1;
		socklen_t add;
		add = sizeof(client);
		//Message Loop
		if (connect(c_sock, (struct sockaddr *)&client, sizeof(client)) >= 0)
		{

			interact(c_sock, add);
		}
		else
		{

			printf("ERROR FOUND AT CONNECTION:\n");
		}
	}
	else
	{

		printf("ERROR FOUND AT SOCKET:\n");
	}

	close(c_sock);
	return 0;
}
