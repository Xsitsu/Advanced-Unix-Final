#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "myioctl.h"
#include "packet.h"

#define BUFF_SIZE 1024

const char *DEVICE_FILE_NAME = "/dev/lab_calc";

int add(int fd, int x, int y);
int sub(int fd, int x, int y);

int timed_accept(int sockfd, struct sockaddr *cli_addr, socklen_t *clilen, int timeout);

int main(int argc, char *argv[])
{
	int fd = open(DEVICE_FILE_NAME, O_RDWR);
	if (fd == -1)
	{
		perror("lab_calc open");
		return 2;
	}

	int bind_port = 40078;

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		fprintf(stderr, "Failed to create socket!\n");
		return -2;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(bind_port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		fprintf(stderr, "Failed to bind socket to port %d!\n", bind_port);
		return -3;
	}

	if (listen(sock_fd, 0) == -1)
	{
		fprintf(stderr, "Failed to listen on socket!\n");
		return -4;
	}

	struct sockaddr_in client_addr;
	socklen_t clilen;

	printf("server up and running on port: %d!\n", bind_port);

	request_t request;
	response_t response;

	char buff[BUFF_SIZE];
	int server_running = 1;
	while (server_running != 0)
	{
		int connection = timed_accept(sock_fd, (struct sockaddr *)&client_addr, &clilen, 1);
		if (connection > 0)
		{
			memset(buff, 0, BUFF_SIZE);
			ssize_t read_size = recv(connection, buff, BUFF_SIZE, 0);
			if (read_size == sizeof(request_t))
			{
				memcpy(&request, buff, sizeof(request_t));
				request.x = ntohl(request.x);
				request.y = ntohl(request.y);

				int x = (int)request.x;
				int y = (int)request.y;

				if (request.op == REQUEST_ADD)
				{
					response.result = htonl((uint32_t)add(fd, x, y));
					response.status = RESPONSE_VALID;
				}
				else if (request.op == REQUEST_SUB)
				{
					response.result = htonl((uint32_t)sub(fd, x, y));
					response.status = RESPONSE_VALID;
				}
				else
				{
					response.status = RESPONSE_ERROR;
				}
			}
			else
			{
				response.status = RESPONSE_ERROR;
			}
			write(connection, &response, sizeof(response_t));
		}
	}

	close(fd);

	return 0;
}

int add(int fd, int x, int y)
{
	query_arg_t q;
	q.num1 = x;
	q.num2 = y;
	q.op = OP_ADD;
        if (ioctl(fd, QUERY_MATH, &q) == -1)
        {
                perror("lab_calc ioctl math add");
        }
	printf("%d + %d = %d\n", x, y, q.result);
	return q.result;
}

int sub(int fd, int x, int y)
{
	query_arg_t q;
	q.num1 = x;
	q.num2 = y;
	q.op = OP_SUB;
        if (ioctl(fd, QUERY_MATH, &q) == -1)
        {
                perror("lab_calc ioctl math sub");
        }
	printf("%d - %d = %d\n", x, y, q.result);
	return q.result;
}

int timed_accept(int sockfd, struct sockaddr *cli_addr, socklen_t *clilen, int timeout)
{
	int result;
	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);

	tv.tv_sec = (long)timeout;
	tv.tv_usec = 0;

	result = select(sockfd+1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
	if(result > 0)
	{
		return accept(sockfd, cli_addr, clilen);
	}
	else
	{
		return result;
	}
}

