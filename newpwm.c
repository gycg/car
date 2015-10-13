/*************************************

NAME:newpwm.c
COPYRIGHT:GYC

*************************************/

#include <linux/module.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <mach/regs-irq.h>
#include <asm/mach/time.h>
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
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <asm/io.h>
#include <plat/regs-timer.h>


#define DEVICE_NAME	"newpwm"
#define DC_MOTOR_MAJOR 102

#define PWM_IOCTL_SET_FREQ      1
#define PWM_IOCTL_STOP          0

struct _freqs
{
        int freq1;
        int freq2;
};

void delay(int m)
{
	int i,j;
	for(i=0;i<m;i++)
		for(j=0;j<34000;j++);
}

static int dc_motor_open(struct inode *inode, struct file *file)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB0,S3C2410_GPB0_TOUT0);
	s3c2410_gpio_cfgpin(S3C2410_GPB1,S3C2410_GPB1_TOUT1);
	//s3c2410_gpio_cfgpin(S3C2410_GPG13,S3C2410_GPG13_OUTP);
        //s3c2410_gpio_cfgpin(S3C2410_GPG14,S3C2410_GPG14_OUTP);
	
	return 0;
}

static int dc_motor_close(struct inode *inode,struct file *file)
{
	return 0;	
}

static void PWM_Set_Freq(unsigned long freq/*struct  _freq freqs*/)
{
	unsigned long tcon;
	unsigned long tcnt;
	unsigned long tcfg1;
	unsigned long tcfg0;

	struct clk *clk_p;
	unsigned long pclk;
	
	s3c2410_gpio_cfgpin(S3C2410_GPB0,S3C2410_GPB0_TOUT0);
	s3c2410_gpio_cfgpin(S3C2410_GPB1,S3C2410_GPB1_TOUT1);

	tcon = __raw_readl(S3C2410_TCON);
	tcfg1 = __raw_readl(S3C2410_TCFG1);
	tcfg0 = __raw_readl(S3C2410_TCFG0);

	tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;
	tcfg0 |= (50-1);

	tcfg1 &= ~S3C2410_TCFG1_MUX0_MASK;
	tcfg1 |= S3C2410_TCFG1_MUX0_DIV16;
	
	__raw_writel(tcfg1, S3C2410_TCFG1);
	__raw_writel(tcfg0, S3C2410_TCFG0);

	clk_p = clk_get(NULL,"pclk");
	pclk = clk_get_rate(clk_p);
	tcnt = (pclk/50/16)/freq;

	__raw_writel(tcnt, S3C2410_TCNTB(0));
	__raw_writel(tcnt*2/5, S3C2410_TCMPB(0));
	__raw_writel(tcnt, S3C2410_TCNTB(1));
	__raw_writel(tcnt*2/5, S3C2410_TCMPB(1));

	tcon &= ~0x1f;
	tcon |= 0xb;
	__raw_writel(tcon,S3C2410_TCON);

	tcon &= ~2;
	__raw_writel(tcon, S3C2410_TCON);
/*	tcon &= ~0xff0;
        tcon |= 0xb00;
        __raw_writel(tcon,S3C2410_TCON);

        tcon &= ~0x600;
        __raw_writel(tcon, S3C2410_TCON);*/
}

static int dc_motor_ioctl(
		struct inode *inode,
		struct file *file,
		unsigned int cmd,
		unsigned long arg)
{
	switch(cmd){

		if(arg == 0)
			return -EINVAL;
		case PWM_IOCTL_SET_FREQ:
			//s3c2410_gpio_setpin(S3C2410_GPG13, 1);
			//s3c2410_gpio_setpin(S3C2410_GPG14, 1);
			PWM_Set_Freq(arg);
			return 0;
	
		case PWM_IOCTL_STOP:
			s3c2410_gpio_setpin(S3C2410_GPB0, 0);
			s3c2410_gpio_setpin(S3C2410_GPB1, 0);
			//s3c2410_gpio_setpin(S3C2410_GPG13, 0);
			//s3c2410_gpio_setpin(S3C2410_GPG14, 0);
        		return 0;

		default:
			return -EINVAL;
		}
}

static struct file_operations dc_motor_fops = {
	.owner =	THIS_MODULE,
	.open =		dc_motor_open,
	.ioctl =	dc_motor_ioctl,
	.release =	dc_motor_close,
};

static int __init dc_motor_init(void)
{
	int ret;
	
	ret = register_chrdev(DC_MOTOR_MAJOR,DEVICE_NAME,&dc_motor_fops);
	if (ret < 0) {
		printk(DEVICE_NAME " can't register major number\n");
		return ret;
	}
	s3c2410_gpio_setpin(S3C2410_GPB0, 0);
	s3c2410_gpio_setpin(S3C2410_GPB1, 0);
	//s3c2410_gpio_setpin(S3C2410_GPG13, 0);
	//s3c2410_gpio_setpin(S3C2410_GPG14, 0);

	printk(DEVICE_NAME " initialized\n");
	return 0;
}

static void __exit dc_motor_exit(void)
{
	unregister_chrdev(DC_MOTOR_MAJOR,DEVICE_NAME);
}

module_init(dc_motor_init);
module_exit(dc_motor_exit);

MODULE_AUTHOR("GYC");
MODULE_DESCRIPTION("TQ2440 dc_motor Driver");
MODULE_LICENSE("GPL");
