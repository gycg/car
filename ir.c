/*************************************

NAME:car.c
COPYRIGHT:GYC

*************************************/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#define PWM_IOCTL_SET_FREQ      1
#define PWM_IOCTL_STOP          0

#define FL1 0
#define FL2 1
#define FR1 2
#define FR2 3

#define LEFT 0
#define RIGHT 1

int OUTIO_fd;
int INIO_fd;
static int PWM_fd = -1;
unsigned long FREQ=5;

struct _freqs
{
        int freq1;
        int freq2;
};
struct _freqs myfreqs;

void openOUTIO(void);
static void close_pwm(void);
static void open_pwm(void);
static void set_pwm(unsigned long freqs);
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

void openINIO(void)
{
        INIO_fd = open("/dev/INIOir",0);
        if(INIO_fd < 0)
        {
                perror("open device INIO");
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

static void set_pwm(unsigned long freqs)
{
        int ret = ioctl(PWM_fd,PWM_IOCTL_SET_FREQ,freqs);
        if(ret < 0)
        {
                perror("set the frequency of the pwm");
                exit(1);
        }
}

void system_init()
{
        openINIO();
        openOUTIO();
        open_pwm();
        forward();
        stop();
}

int check_by_way(char pin_state,int direction)
{
        if(pin_state == '0')
        {
                if(direction == LEFT)
                {
                        turn_right();
                        set_pwm(FREQ);
                        printf("LEFT Is ON and Now Turn RIGHT!\n");
                        sleep(1);
                }
                else
                {
                        turn_left();
                        set_pwm(FREQ);
                        printf("RIGHT Is ON and Now Turn LEFT!\n");
                        sleep(1);
                }
                forward();
		set_pwm(FREQ);
                printf("********Go Forward!********\n");
        }
        return 0;
}

int getway(void)
{
        char current_INIO[2];
        int now_turn=0;
        unsigned char Flag;
        //int *tmp;
        read(INIO_fd,current_INIO,sizeof current_INIO);
        //Flag = tohex(current_INIO,2);
        check_by_way(current_INIO[0],LEFT);
        check_by_way(current_INIO[1],RIGHT);
}

int main(int argc,char **argv)
{
        system_init();
        forward();
        set_pwm(FREQ);
        printf("********Go Forward!********\n");
        while(1)
        {
                getway();
        }
}

