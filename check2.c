#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#define PWM_IOCTL_SET_FREQ      1
#define PWM_IOCTL_STOP          0
#define BIGLEFT 0
#define LEFT 1
#define NOTURN 2
#define RIGHT 3
#define BIGRIGHT 4
#define FL1 0
#define FL2 1
#define FR1 2
#define FR2 3
#define WHITE_LINE 1
#define Freq 100

int PWM_fd;
int OUTIO_fd;
int INIO_fd;

struct _freqs
{
        int freq_left;
        int freq_right;
        int width_left;
        int width_right;
};
void openINIO(void);
void set_pwm(int f_left,int w_left,int f_right,int w_right);
static void set_pwm_freq(struct _freqs freqs);
void openOUTIO(void);
static void close_pwm(void);
static void open_pwm(void);
void forward();
void back();
void turn_left();
void turn_right();
unsigned char tohex(unsigned char *IN,int N);

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

static void stop_pwm(void)
{
	forward();
	int ret = ioctl(PWM_fd,PWM_IOCTL_STOP);
	if(ret<0)
	{
		perror("stop the pwm");
		exit(1);
	}
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
void openINIO(void)
{
        INIO_fd = open("/dev/INIO",0);
        if(INIO_fd < 0)
        {
                perror("open device INIO");
                exit(1);
        }
}
void getway(void)
{
        char current_INIO[6];
        while(1)
        {
                read(INIO_fd,current_INIO,sizeof current_INIO - 1);
                current_INIO[5] = '\0';
                printf("Check Result Is : %s \n",current_INIO);
                sleep(1);
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
static void open_pwm(void)
{
	PWM_fd = open("/dev/newpwm",0);
	if(PWM_fd < 0)
	{
		perror("open pwm_pwm device");
		exit(1);
	}
	atexit(close_pwm);
	return;
}
static void close_pwm(void)
{
	forward();
	if (PWM_fd >= 0)
	{
		ioctl(PWM_fd,PWM_IOCTL_STOP);
		close(PWM_fd);
		PWM_fd = -1;	
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
int check_which_turn(unsigned char Flag)
{
	char RTurn,LTurn,Turn;
	int i;
	static itn tmp=0;
	RTurn = NOTURN;
	LTurn = NOTURN;
	Turn = NOTURN;
	for(i=2;i>=0;i--)
	{
		if((Flag>>i) & 0x01)
			LTurn = 1;
	}
	for(i=2;i<=4;i++)
	{
		if((Flag>>i) & 0x01)
			RTurn = i;
	}
	if((RTurn-4+LTurn)>=0)
		Turn = RTurn;
	else
		Turn = LTurn;
	if(Flag == 0x00)
	{
		Turn = last_turn;
		LSpeed = 100;
		RSpeed = 100;
	}
	else
	{
		LSpeed = 600;
		RSpeed = 600;
	}
	return Turn;
}

int Get_M_Line(int Times,unsigned char Turn)
{
	char current_INIO[9];
	unsigned char Flag;
	int now_turn;	
	while(Time--)
	{
		read(INIO_fd,current_INIO,sizeof current_INIO);
		Flag = tohex(current_INIO,5);
		now_turn = check_which_turn(Flag);
		if((last_turn>=RIGHT) && (now_turn<RIGHT)) return 1;
		if((last_turn<=LEFT) && (now_turn>LEFT)) return 1;
		if(now_turn == NOTURN)
		{
			usleep(2000);
			return 1;
		}
		if(Turn>=2)
			if(now_turn > Turn)
				return 1;
		if(Turn<=2)
			if(now_turn < Turn)
				return 1;
		usleep(1000);
	}
	return 0;
}
void turn(int turn_flag)
{
	long int cn=0;
	if(turn_flag == BIGLEFT)
	{
		printf("Turn BIGLEFT\n");
		turn_big_left();
		set_pwm(Freq,300,Freq,900);
		Get_M_Line(5000,turn_flag);
	}
	esle
	if(turn_flag == BIGRIGHT)
	{
		printf("Turn BIGRIGHT\n");
		turn_big_right();
		set_pwm(Freq,990,Freq,300);
		Get_M_Line(5000,turn_falg);
	}
	else
	if(turn_falg == LEFT)
	{
		printf("Turn LEFT\n");	
		turn_left();	
		set_pwm(Freq,400,Freq,990);
		Get_M_Line(5000,turn_flag);
	}
	else
	if(turn_flag == RIGHT)
	{
		printf("Turn RIGHT\n");
		turn_right();	
		set_pwm(Freq,990,Freq,400);
		Get_M_Line(5000,turn_flag);
	}
	else
	if(turn_flag == NOTURN)
	{
		printf("NOTURN\n");
		forward();
		set_pwm(Freq,LSpeed,Freq,RSpeed);
		delay(10);
		len_way++;
	}
	last_turn = turn_flag;
}
	
int check_flag(int flag)
{
	if(EHITE_LINE)
		return((~flag) & 0x1FF);
	else
		return flag;
}

unsigned char tohex(unsigned char *IN,int N)
{
	int i;
	int result;
	result = 0;
	for(i=0;i<N;i++)
	{
		if(IN[i] == '1')
		{
			result |= (1<<i);
		}
	}
	result = check_flag(result);
	return result;
}

void getway(void)
{
	char current_INIO[6];
	int now_turn = 0;
	unsigned char Flag;
	while(1)
	{
		read(INIO_fd,current_INIO,sizeof current_INIO-1);

		current_INIO[5] = '\0';
	printf("Check Result Is : %s \n",current_INIO);
	Flag = tohex(current_INIO,5);
	now_turn = check_which_turn(Flag);
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
