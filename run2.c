/*************************************

NAME:run2.c
COPYRIGHT:GYC

*************************************/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>

#define PWM_IOCTL_SET_FREQ      1
#define PWM_IOCTL_STOP          0
#define LEFT -1
#define REGHT 1
#define NOTURN 0
#define BIGLEFT -2
#define BIGRIGHT 2

#define FL1 1
#define FL2 7
#define FR1 9 
#define FR2 10
int OUTIO_fd;

struct _freqs
{
        int freq_left;
        int freq_right;
	int width_left;
	int width_right;
};
static int PWM_fd = -1;

void openOUTIO(void);
static void close_pwm(void);
static void open_pwm(void);
static void set_pwm_freq(struct _freqs freqs);
void forward();
void back();
void turn_left();
void turn_right();

void openOUTIO(void)
{
	OUTIO_fd = open("/dev/OUTIO", 0);
	if (OUTIO_fd < 0)
	{
		perror("open device OUTIO");
		exit(1);
	}
}

static void open_pwm(void)
{
	PWM_fd =  open("/dev/newpwm",0);
	if(PWM_fd < 0)
	{
		perror("open pwm device");
		exit(1);
	}
	atexit(close_pwm);
	return;
}

static void close_pwm(void)
{
	if(PWM_fd >= 0)
	{
		ioctl(PWM_fd,PWM_IOCTL_STOP);
		close(PWM_fd);	
		PWM_fd = -1;
	}
}

void forward()
{
	ioctl(OUTIO_fd,0,FL1);
	ioctl(OUTIO_fd,1,FL2);
	ioctl(OUTIO_fd,0,FR1);
	ioctl(OUTIO_fd,1,FR2);
}
void back()
{
	ioctl(OUTIO_fd,1,FL1);
	ioctl(OUTIO_fd,0,FL2);
	ioctl(OUTIO_fd,1,FR1);
	ioctl(OUTIO_fd,0,FR2);
}
void turn_left()
{
	ioctl(OUTIO_fd,1,FL1);
	ioctl(OUTIO_fd,0,FL2);
	ioctl(OUTIO_fd,0,FR1);
	ioctl(OUTIO_fd,1,FR2);
}
void turn_right()
{
	ioctl(OUTIO_fd,0,FL1);
	ioctl(OUTIO_fd,1,FL2);
	ioctl(OUTIO_fd,1,FR1);	
	ioctl(OUTIO_fd,0,FR2);
}

void set_pwm(int f_left,int w_left,int f_right,int w_right)
{
	struct _freqs myfreqs;
	myfreqs.freq_left = f_left;
	myfreqs.width_left = w_left;
	myfreqs.freq_right = f_right;
	myfreqs.width_right = w_right;
	int ret = ioctl(PWM_fd,PWM_IOCTL_SET_FREQ,&myfreqs);
	if(ret < 0)
	{
		perror("set the PWM");	
		exit(1);
	}
}

void system_init()
{
	open_pwm();
	openOUTIO();
	forward();
//	stop();
}

int main(int argc, char **argv)
{
	int Freq = 200;
	system_init();
	while(1)
	{
		forward();
		set_pwm(Freq,400,Freq,400);
		printf("Go Forward!\n");
		sleep(4);
		turn_left();
		set_pwm(Freq,400,Freq,400);
		printf("Turn Left!\n");
		sleep(2);	
		turn_right();
		set_pwm(Freq,300,Freq,300);
		printf("Turn Right \n");
		sleep(2);
	}
}
