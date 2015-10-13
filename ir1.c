/*************************************

NAME:ir1.c
COPYRIGHT:GYC

*************************************/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
int OUTIO_fd;
int INIOir_fd;

void openOUTIO(void)
{
	OUTIO_fd = open("/dev/OUTIO",0);
	if(OUTIO_fd < 0)
	{
		perror("open device OUTIO");
		exit(1);
	}
}

void openINIOir(void)
{
	INIOir_fd = open("/dev/INIOir",0);
	if(INIOir_fd < 0)
	{
		perror("open device INIOir");
		exit(1);
	}
}
void system_init()
{
	openINIOir();
	openOUTIO();
}

int main(int argc,char **argv)
{
	char current_INIOir[2];
	system_init();
	while(1)
	{
		read(INIOir_fd,current_INIOir,sizeof current_INIOir);
		if(current_INIOir[0]=='0')
		{
			printf("LEFT Is ON!\n");
		}
		sleep(1);
		if(current_INIOir[1]=='0')
		{
			printf("RIGHT Is ON!\n");
		}
		sleep(1);
	}
}
