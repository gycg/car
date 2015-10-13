/*************************************

NAME:18B20.c
COPYRIGHT:GYC

*************************************/
#include <linux/module.h>
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
#include <linux/gpio.h>

typedef unsigned char BYTE;

#define DS18B20_PIN S3C2410_GPG13
#define DS18B20_PIN_OUTP S3C2410_GPG13_OUTP
#define DS18B20_PIN_INP S3C3410_GPG13_INP

#define HIGH 1
#define LOW 0
#define DEVICE_NAME "DS18B20"
static BYTE data[2];

BYTE DS18b20_reset(void)
{
	static int k=0;
	s3c2410_gpio_cfgpin(DS18B20_PIN,DS18B20_PIN_OUTP);
	s3c2410_gpio_setpin(DS18B20_PIN,HIGH);
	udelay(100);
        s3c2410_gpio_setpin(DS18B20_PIN,LOW);

        udelay(100);
	
        s3c2410_gpio_cfgpin(DS18B20_PIN,DS18B20_PIN_INP);
	if(k++<10)
	if(s3c2410_gpio_getpin(DS18B20_PIN))
	{
		return 1;
	}
	k = 0;
	return 0;
}

void DS18b20_write_byte(BYTE byte)
{
	BYTE i;
	s3c2410_gpio_cfgpin(DS18B20_PIN,DS18B20_PIN_OUTP);
	for(i = 0;i < 8;i++)
	{
		s3c2410_gpio_setpin(DS18B20_PIN,LOW);
	        udelay(1);
		if(byte & HIGH)
		{
			s3c2410_gpio_setpin(DS18B20_PIN,HIGH);
		}
		else
		{
			s3c2410_gpio_setpin(DS18B20_PIN,LOW);
		}
		udelay(60);
		s3c2410_gpio_setpin(DS18B20_PIN,HIGH);
		udelay(15);
		byte >>=1;
	}
	s3c2410_gpio_setpin(DS18B20_PIN,HIGH);
}

BYTE DS18b20_read_byte(void)
{
	BYTE i = 0;
	BYTE byte = 0;
	for(i = 0;i < 8;i++)
	{
		s3c2410_gpio_cfgpin(DS18B20_PIN,DS18B20_PIN_OUTP);
		s3c2410_gpio_setpin(DS18B20_PIN,LOW);
		udelay(1);
		byte >>=1;
		s3c2410_gpio_setpin(DS18B20_PIN,HIGH);
		s3c2410_gpio_cfgpin(DS18B20_PIN,DS18B20_PIN_INP);
		
		if(s3c2410_gpio_getpin(DS18B20_PIN)) byte |= 0x80;
		udelay(60);
	}
	return byte;
}

void DS18b20_proc(void)
{
	float result = 25.2;
	while(DS18b20_reset());
	udelay(120);
	DS18b20_write_byte(0xcc);
	DS18b20_write_byte(0x44);
	udelay(5);

	while(DS18b20_reset());
        udelay(200);

	DS18b20_write_byte(0xcc);
        DS18b20_write_byte(0xbe);

	data[0] = DS18b20_read_byte();
	data[1] = DS18b20_read_byte();

static int s3c2440_18b20_read(
		struct file *file,
		char __user *buff,
		size_t count,
		loff_t *offp)
{
	DS18b20_proc();
	buff[0] = data[0];
	buff[1] = data[1];
	return 1;
}
	
static struct file_operations s3c2440_18b20_fops =
{
	.owner = THIS_MODULE,
	.read = s3c2440_18b20_read,
};

static struct miscdevice misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &s3c2440_18b20_fops,
};

static int __init s3c2440_18b20_init(void)
{
	int ret;
	ret = misc_register(&misc);
	printk(DEVICE_NAME"\tinitialized\n");
	return ret;
	while(DS18b20_reset());
}

static void __exit s3c2440_18b20_exit(void)
{
	misc_deregister(&misc);
}
module_init(s3c2440_18b20_init);
module_exit(s3c2440_18b20_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GYC");
