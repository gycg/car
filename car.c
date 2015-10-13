/************************************

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

#define FREQ 100

#define PWM_IOCTL_SET_FREQ      1
#define PWM_IOCTL_STOP          0

#define FL1 0
#define FL2 1
#define FR1 2
#define FR2 3

#define LEFT -1
#define RIGHT 1
#define NOTURN 0
#define BIGLEFT -2
#define BIGRIGHT 2

int last_turn = 0;
int OUTIO_fd;
int INIO_fd;
static int PWM_fd = -1;

/*struct _freqs
{
        int freq1;
        int freq2;
};
struct _freqs myfreqs;
*/
void openINIO(void);
void openOUTIO(void);
static void close_pwm(void);
static void open_pwm(void);
static void set_pwm(unsigned long freqs);
void forward();
void back();
void turn_left();
void turn_right();
void turn_big_left();
void turn_big_right();
void stop();

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
        INIO_fd = open("/dev/INIO",0);
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
static void set_pwm(unsigned long freqs)
{
        int ret = ioctl(PWM_fd,PWM_IOCTL_SET_FREQ,freqs);
        if(ret < 0)
        {
                perror("set the frequency of the pwm");
                exit(1);
        }
}
static void stop_pwm(void)
{
        forward();
        int ret = ioctl(PWM_fd,PWM_IOCTL_STOP);
        if(ret < 0)
        {
                perror("stop the pwm");
                exit(1);
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
        ioctl(OUTIO_fd,0,FL1);
        ioctl(OUTIO_fd,0,FL2);
        ioctl(OUTIO_fd,0,FR1);
        ioctl(OUTIO_fd,1,FR2);
}
void turn_right()
{
        ioctl(OUTIO_fd,0,FL1);
        ioctl(OUTIO_fd,1,FL2);
        ioctl(OUTIO_fd,0,FR1);
        ioctl(OUTIO_fd,0,FR2);
}
void turn_big_left()
{
        ioctl(OUTIO_fd,1,FL1);
        ioctl(OUTIO_fd,0,FL2);
        ioctl(OUTIO_fd,0,FR1);
        ioctl(OUTIO_fd,1,FR2);
}
void turn_big_right()
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

void system_init()
{
        openINIO();
        openOUTIO();
        open_pwm();
        forward();
        stop();
}
void delay(int m)
{
        int i,j;
        for(i=0;i<m;i++)
                for(j=0;j<34000;j++);
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

int check_which_turn(char *flag)
{
	if(!strcmp(flag,"11111"))
                return NOTURN;
        else if(!strcmp(flag,"11110") | !strcmp(flag,"11010"))
                return BIGLEFT;
        else if(!strcmp(flag,"11101") | !strcmp(flag,"11001") |
                !strcmp(flag,"11000") | !strcmp(flag,"11100"))
                return LEFT;
        else if(!strcmp(flag,"01111") | !strcmp(flag,"01011"))
                return BIGRIGHT;
        else if(!strcmp(flag,"10011") | !strcmp(flag,"10111") |
                !strcmp(flag,"00111") | !strcmp(flag,"00011"))
                return RIGHT;
        else return NOTURN;
}
void turn(int turn_flag)
{
        long int cn  = 0;
        if(turn_flag == BIGLEFT)
        {
                printf("BIGLEFT\n");
                turn_big_left();
                delay(10);
                set_pwm(FREQ);
                delay(5);
        }
        else
        if(turn_flag == BIGRIGHT)
        {
                printf("BIGRIGHT\n");
                turn_big_right();
                delay(10);
                set_pwm(FREQ);
                delay(5);
        }
        else
        if(turn_flag == LEFT)
        {
                printf("LEFT\n");
                turn_left();
                delay(10);
                set_pwm(FREQ);
                delay(5);
        }
        else
        if(turn_flag == RIGHT)
        {
                printf("RIGHT\n");
                turn_right();
                delay(10);
                set_pwm(FREQ);
                delay(5);
        }
        else
        if(turn_flag == NOTURN)
        {
                printf("NOTURN\n");
                forward();
                delay(10);
                set_pwm(FREQ);
 		delay(5);
        }
}
int getway(void)
{
	int i;
        int now_turn=0;
        char current_INIO[7];
        unsigned char Flag;
        while(1)
	{
		read(INIO_fd,current_INIO,7);
        	check_by_way(current_INIO[5],LEFT);
	        check_by_way(current_INIO[6],RIGHT);
		current_INIO[5] = '\0';
                for(i=0;i<5;i++)
                {
                        if(current_INIO[i]=='0')
                                current_INIO[i] = '1';
                        else
                                current_INIO[i] = '0';
                }
                printf("Check Result Is : %s \n",current_INIO);
		now_turn = check_which_turn(current_INIO);
                turn(now_turn);
	}
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
