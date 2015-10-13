/*************************************

NAME:INIO.c
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

#define DEVICE_NAME     "INIO"
#define INIO_MAJOR     103

static unsigned long INIO_table[] = {
    S3C2410_GPG0,
    S3C2410_GPG3,
    S3C2410_GPG7,
    S3C2410_GPG10,
    S3C2410_GPG11,
    S3C2410_GPG13,
    S3C2410_GPG14,
};

static unsigned int INIO_cfg_table[] = {
    S3C2410_GPG0_INP,
    S3C2410_GPG3_INP,
    S3C2410_GPG7_INP,
    S3C2410_GPG10_INP,
    S3C2410_GPG11_INP,
    S3C2410_GPG13_INP,
    S3C2410_GPG14_INP,
};

static int INIO_open(struct inode *inode, struct file *file)
{
        int i;

        for (i = 0; i < 5; i++){
                s3c2410_gpio_cfgpin(INIO_table[i], INIO_cfg_table[i]);
        }
        return 0;
}

static int INIO_read(
	struct file *file,
	char __user *buff,
	size_t count,
	loff_t *offp)
{
	int i;
	for(i=0; i<count; i++)
		sprintf(&buff[i],"%d",s3c2410_gpio_getpin(INIO_table[i]));
	return 0;
}

static struct file_operations INIO_fops = {
        .owner  =       THIS_MODULE,
        .open   =       INIO_open,
        .read   =       INIO_read,
};

static int __init INIO_init(void)
{
        int ret;

        ret = register_chrdev(INIO_MAJOR, DEVICE_NAME, &INIO_fops);
        if (ret < 0) {
                printk(DEVICE_NAME " can't register major number\n");
                return ret;
        }

        printk(DEVICE_NAME " initialized\n");
        return 0;
}

static void __exit INIO_exit(void)
{
        unregister_chrdev(INIO_MAJOR, DEVICE_NAME);
}

module_init(INIO_init);
module_exit(INIO_exit);

MODULE_AUTHOR("GYC");
MODULE_DESCRIPTION("INIO Driver");
MODULE_LICENSE("GPL");
