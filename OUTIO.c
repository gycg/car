/*************************************

NAME:OUTIO.c
COPYRIGHT:GYC

*************************************/

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#define DEVICE_NAME	"OUTIO"
#define OUTIO_MAJOR	101

void delay(int m)
{
        int i,j;
        for(i=0;i<m;i++)
                for(j=0;j<34000;j++);
}

static unsigned long OUTIO_table[] = {
    S3C2410_GPF3,
    S3C2410_GPF4,
    S3C2410_GPG5,
    S3C2410_GPG6,
};

static unsigned int OUTIO_cfg_table[] = {
    S3C2410_GPF3_OUTP,
    S3C2410_GPF4_OUTP,
    S3C2410_GPG5_OUTP,
    S3C2410_GPG6_OUTP,
};

static int OUTIO_open(struct inode *inode, struct file *file)
{
	int i;

	for (i = 0; i < 4; i++){
		s3c2410_gpio_cfgpin(OUTIO_table[i], OUTIO_cfg_table[i]);
	}
	return 0;
}

static int OUTIO_ioctl(
	struct inode *inode,
    	struct file *file,
    	unsigned int cmd,
   	unsigned long arg)
{
	if(arg > 4){
		return -EINVAL;
	}
	switch(cmd){
	case 0:
		s3c2410_gpio_setpin(OUTIO_table[arg],0);
		delay(1000);
		return 0;

	case 1:
                s3c2410_gpio_setpin(OUTIO_table[arg],1);
		delay(1000);
                return 0;

	default:
        	return -EINVAL;
	}
}

static struct file_operations OUTIO_fops = {
	.owner 	=	THIS_MODULE,
	.open	=	OUTIO_open,
	.ioctl	=	OUTIO_ioctl,
};

static int __init OUTIO_init(void)
{
	int ret;
	
	ret = register_chrdev(OUTIO_MAJOR, DEVICE_NAME, &OUTIO_fops);
	if (ret < 0) {
		printk(DEVICE_NAME " can't register major number\n");
		return ret;
    	}

	printk(DEVICE_NAME " initialized\n");
	return 0;
}

static void __exit OUTIO_exit(void)
{
	unregister_chrdev(OUTIO_MAJOR, DEVICE_NAME);
}

module_init(OUTIO_init);
module_exit(OUTIO_exit);

MODULE_AUTHOR("GYC");
MODULE_DESCRIPTION("OUTIO Driver");
MODULE_LICENSE("GPL");
