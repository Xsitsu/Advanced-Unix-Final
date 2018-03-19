#ifndef _MYIOCTL_H
#define _MYIOCTL_H
#include <linux/ioctl.h>

#define OP_ADD 1
#define OP_SUB 2

typedef struct
{
	int num1;
	int num2;
	int op;
	int result;
} query_arg_t;

#define QUERY_MATH _IOR('q', 1, query_arg_t *)

#endif

