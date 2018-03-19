#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "myioctl.h"

void set_null(int fd)
{
	if (ioctl(fd, QUERY_SET_NULL) == -1)
	{
		perror("query_apps ioctl set null");
	}
}

void set_A(int fd)
{
	if (ioctl(fd, QUERY_SET_A) == -1)
	{
		perror("query_apps ioctl set a");
	}
}

void get_status(int fd)
{
	query_arg_t q;
	if (ioctl(fd, QUERY_GET_STATUS, &q) == -1)
	{
		perror("query_apps ioctl get status");
	}
	else
	{
		printf("Status: %d\n", q.status);
	}
}

void usage(char *prog_name)
{
	fprintf(stderr, "Usage: %s [-s | -n | -a]\n", prog_name);
}

int main(int argc, char *argv[])
{
	char *file_name = "/dev/query";
	int fd;
	enum
	{
		e_status,
		e_set_null,
		e_set_a
	} option;

	if (argc == 2)
	{
		if (strcmp(argv[1], "-s") == 0)
		{
			option = e_status;
		}
		else if (strcmp(argv[1], "-n") == 0)
		{
			option = e_set_null;
		}
		else if (strcmp(argv[1], "-a") == 0)
		{
			option = e_set_a;
		}
		else
		{
			usage(argv[0]);
            		return 1;
		}
	}
	else
	{
		usage(argv[0]);
		return 1;
	}

	fd = open(file_name, O_RDWR);
	if (fd == -1)
	{
		perror("query_apps open sdfsdfdsf");
		return 2;
	}

	switch (option)
	{
	case e_status:
		get_status(fd);
		break;
	case e_set_null:
		set_null(fd);
		break;
	case e_set_a:
		set_A(fd);
		break;
	default:
		break;
	}

	close(fd);

	return 0;
}
