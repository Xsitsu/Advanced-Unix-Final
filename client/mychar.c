#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

#include "myioctl.h"

// MODULE Definitions
#define FIRST_MINOR 0
#define MINOR_CNT 1
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacob Locke");
MODULE_DESCRIPTION("lab final driver");

// Variables
static dev_t majorNum;
static struct cdev CharDevice;
static struct class * CharClass;

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

#define BUFF_SIZE 512
static char buffer[BUFF_SIZE];
static int buff_len;


// Operational Functions
static int mycharOpen(struct inode * i, struct file * f)
{
	printk(KERN_INFO "lab final Driver: Open Function\n");
	return 0;
}

static int mycharClose(struct inode * i,  struct file * f)
{
	printk(KERN_INFO "lab final Driver: Close Function\n");
	return 0;
}

static ssize_t mycharRead(struct file * f, char __user * buf, size_t len, loff_t * offset)
{
	int send_len;

	printk(KERN_INFO "lab final Driver: Read Function\n");

	// Send my buffer to userland
	if(*offset  ==  0)
	{
		send_len = buff_len;
		if (send_len > len)
		{
			send_len = len;
		}

		printk(KERN_INFO "lab final Driver: len: %d; send_len: %d\n", len, send_len);

		if (send_len > 0)
		{
			if(copy_to_user(buf, buffer, send_len) != 0)
			{
				return -EFAULT;
			}
			else
			{
				(*offset)++;
				return send_len;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

static ssize_t mycharWrite(struct file * f, const char __user * buf, size_t len, loff_t * offset)
{
	int write_len;

	printk(KERN_INFO "lab final Driver: Write Function\n");

	write_len = len;
	if (write_len > BUFF_SIZE)
	{
		write_len = BUFF_SIZE;
	}

	// Copy last char from userland to my buffer
	if(copy_from_user(buffer, buf + len - write_len, write_len) != 0)
	{
		return -EFAULT;
	}
	else
	{
		buff_len = write_len;
		return len;
	}
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	query_arg_t q;

	printk(KERN_INFO "lab final driver: ioctl function called\n");

	switch(cmd)
	{
	case QUERY_MATH:
		if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)) != 0)
		{
			return -EFAULT;
		}

		switch (q.op)
		{
		case OP_ADD:
			q.result = q.num1 + q.num2;
			break;
		case OP_SUB:
			q.result = q.num1 - q.num2;
			break;
		default:
			return-EINVAL;
		}

		if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
		{
			return -EACCES;
		}
        	break;
	default:
		return -EINVAL;
	}
	return 0;
}


// Setup fops
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = mycharOpen,
	.release = mycharClose,
	.read = mycharRead,
	.write = mycharWrite,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = my_ioctl
#else
	.unlocked_ioctl = my_ioctl
#endif
};

// Driver Init Function
static int __init mycharInit(void)
{
	int ret;
	struct device *dev_ret;

	printk(KERN_INFO "lab final Driver: Registered\n");

	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
	{
		return ret;
	}

	cdev_init(&c_dev, &fops);

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	{
		return ret;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query")))
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	// Attempt to Register Character device
	if(alloc_chrdev_region(&majorNum, 0, 1, "Jacob") < 0)
	{
		return -1;
	}

	// Attempt to create the device class for udev
	if((CharClass = class_create(THIS_MODULE, "chardrv"))  ==  NULL)
	{
		unregister_chrdev_region(majorNum, 1);
		return -1;
	}

	// Create the actual device in /dev folder
	if(device_create(CharClass, NULL, majorNum, NULL, "query")  ==  NULL)
	{
		class_destroy(CharClass);
		unregister_chrdev_region(majorNum, 1);
		return -1;
	}

	// Initialize the CHAR device
	cdev_init(&CharDevice, &fops);
	if(cdev_add(&CharDevice, majorNum, 1)  ==  -1)
	{
		device_destroy(CharClass, majorNum);
		class_destroy(CharClass);
		unregister_chrdev_region(majorNum, 1);
		return -1;
	}

	memset(buffer, 0, BUFF_SIZE);
	buff_len = 0;

	return 0;
}

static void __exit mycharExit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);

	cdev_del(&CharDevice);
	device_destroy(CharClass, majorNum);
	class_destroy(CharClass);
	unregister_chrdev_region(majorNum, 1);
	printk(KERN_INFO "lab final Driver: Unregistered\n");
}

module_init(mycharInit);
module_exit(mycharExit);

