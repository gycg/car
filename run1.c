/*************************************

NAME:run1.c
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

#define FL1 0
#define FL2 1
#define FR1 2 
#define FR2 3
int OUTIO_fd;

struct _freqs
{
        int freq1;
        int freq2;
};
struct _freqs myfreqs;
static int PWM_fd = -1;

void openOUTIO(void);
static void close_pwm(void);
static void open_pwm(void);
static void set_pwm_freq(struct _freqs freqs);
void forward();
void back();
void turn_left();
void turn_right();

void delay(int m)
{
        int i,j;
        for(i=0;i<m;i++)
                for(j=0;j<34000;j++);
}

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

void stop()
{
        ioctl(OUTIO_fd,0,FL1);
        ioctl(OUTIO_fd,0,FL2);
        ioctl(OUTIO_fd,0,FR1);
        ioctl(OUTIO_fd,0,FR2);
}

static void set_pwm_freq(struct _freqs freqs)
{
	int ret = ioctl(PWM_fd,PWM_IOCTL_SET_FREQ,&freqs);
	if(ret < 0)
	{
		perror("set the frequency of the pwm");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	myfreqs.freq1 = 200;
	myfreqs.freq2 = 200;
	open_pwm();
	openOUTIO();
	while(1)
	{
		forward();
		set_pwm_freq(myfreqs);
		printf("forward!\n");
		sleep(2);
		back();
		set_pwm_freq(myfreqs);
		printf("back!\n");
		sleep(2);
		stop();
		set_pwm_freq(myfreqs);
		printf("stop!\n");
		sleep(2);
		turn_left();
		set_pwm_freq(myfreqs);
		printf("turn left!\n");
		sleep(2);
		turn_right();
		set_pwm_freq(myfreqs);
		printf("turn right!\n");
		sleep(2);
	}
}
