/*************************************

NAME:distance1.c
COPYRIGHT:GYC

*************************************/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

int PWM_fd;
int OUTIOdis_fd;
int INIOdis_fd;

#define DIS 6
#define LOW 0
#define HIGHT 1

void openOUTIOdis(void)
{
        OUTIOdis_fd = open("/dev/OUTIOdis", 0);
        if (OUTIOdis_fd < 0)
        {
                perror("open device OUTIO");
                exit(1);
        }
}

openINIOdis(void)
{
	INIOdis_fd = open("/dev/INIOdis",0);
	if(INIOdis_fd < 0)
	{
		perror("open device INIOdis");
		exit(1);
	}
}
	
int get_dis(void)
{
	long int out_n,out_n2;
	char current_INIO[1];
	struct timeval start,end;
	int timeuse;

	timeuse = 10000;
	out_n = 10000;
	read(INIOdis_fd,current_INIO,sizeof current_INIO);
	printf("Here1 \n");
	ioctl(OUTIOdis_fd,LOW,DIS);
	usleep(10);
	ioctl(OUTIOdis_fd,HIGHT,DIS);
	usleep(10);
	ioctl(OUTIOdis_fd,LOW,DIS);
	printf("Here \n");
	
	while((out_n > 0) && (current_INIO[0] != '1'))
	{
	out_n--;
	read(INIOdis_fd,current_INIO,sizeof current_INIO);
	}
	if(out_n > 0)
	{
	out_n = 2*1000;
	gettimeofday(&start,NULL);
	
	while(1)
	//while(((out_n--)>0) && current_INIO[0] == '1')
	{	
		read(INIOdis_fd,current_INIO,sizeof current_INIO);
	}
	gettimeofday(&end,NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse = 0.17*timeuse;
	}
	if(timeuse > 5)
		return (timeuse);
	else
		return 4000;
} 

void system_init()
{
	openINIOdis();
	openOUTIOdis();
}

int main(int argc,char **argv)
{
	int dis=0;
	system_init();
	while(1)
	{
		dis = get_dis();
		printf("Distance is %dmm \n",dis);
		sleep(1);
	}
}
