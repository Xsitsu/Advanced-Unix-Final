#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "myioctl.h"

const char *DEVICE_FILE_NAME = "/dev/lab_calc";

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
	return q.result;
}

int main(int argc, char *argv[])
{
	int fd;

	fd = open(DEVICE_FILE_NAME, O_RDWR);
	if (fd == -1)
	{
		perror("query_apps open sdfsdfdsf");
		return 2;
	}

	int res = add(fd, 2, 3);
	printf("2 + 2 = %d\n", res);
	res = sub(fd, 20, 5);
	printf("20 - 5 = %d\n", res);

	close(fd);

	return 0;
}
