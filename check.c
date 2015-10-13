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
int PWM_fd;
int OUTIO_fd;
int INIO_fd;

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
void openINIO(void);

void openOUTIO(void)
{
        OUTIO_fd = open("/dev/OUTIO",0);
        if(OUTIO_fd < 0)
        {
                perror("open device OUTIO");
                exit(1);
        }
}
static void open_pwm(void)
{
        PWM_fd = open("/dev/newpwm",0);
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

void openINIO(void)
{
        INIO_fd = open("/dev/INIO",0);
        if(INIO_fd < 0)
        {
                perror("open device INIO");
                exit(1);
        }
}
void system_init()
{
        open_pwm();
        openINIO();
        openOUTIO();
        forward();
        stop();
}
void delay(int m)
{
        int i,j;
        for(i=0;i<m;i++)
                for(j=0;j<34000;j++);
}
void delayus(int m)
{
        int i,j;
        for(i=0;i<m;i++)
                for(j=0;j<34;j++);
}
int check_which_turn(char *flag)
{
/*	if((flag & (1<<2))) return NOTURN;
	if(flag == 1<<3) return RIGHT;
	if(flag == 1<<1) return LEFT;
	if(flag & (1<<4)) return BIGRIGHT;
	if(flag & (1<<0)) return BIGLEFT;	
	if(flag == ((1<<1) | (1<<2))) return LEFT;
	if(flag == ((1<<2) | (1<<3))) return RIGHT;
	
	if((flag == 0) && ((last_turn==BIGLEFT) | (last_turn==BIGRIGHT)))
		return last_turn;
	return NOTURN;*/
	if(!strcmp(flag,"00000"))
		return NOTURN;
	else if(!strcmp(flag,"00001") | !strcmp(flag,"00101"))
		return BIGLEFT; 
	else if(!strcmp(flag,"00010") | !strcmp(flag,"00110") | 
		!strcmp(flag,"00111") | !strcmp(flag,"00011"))
		return LEFT; 
	else if(!strcmp(flag,"10000") | !strcmp(flag,"10100"))
                return BIGRIGHT;
	else if(!strcmp(flag,"01100") | !strcmp(flag,"01000") |
                !strcmp(flag,"11000") | !strcmp(flag,"11100"))
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
	//last_turn = turn_flag;
}

/*unsigned char tohex(unsigned char *IN,int N)
{
        int i;
        int result;
        result = 0;
        for(i=0;i<N;i++)
        {
                if(IN[i] == '1')
                {
                        result |= (1<<(4-i));
                }
        }
        return result;
}*/
void getway(void)
{
	int i;
	char current_INIO[6];
	int now_turn = 0;
	//unsigned char Flag;
	while(1)
	{
		read(INIO_fd,current_INIO,sizeof current_INIO-1);
		current_INIO[5] = '\0';
		for(i=0;i<5;i++)
		{
			if(current_INIO[i]=='0')
				current_INIO[i] = '1';
			else 
				current_INIO[i] = '0';
		}
		//Flag = tohex(current_INIO,5);
		printf("Check Result Is : %s \n",current_INIO);
		//printf("Check Result Is : %o \n",Flag);
		now_turn = check_which_turn(current_INIO);
	        turn(now_turn);
	}
}
int main(int argc,char **argv)
{
        system_init();
        while(1)
        {
                getway();
        }
}
