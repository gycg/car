#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>

int fd_18b20;
void init_18b20();
float get_Tc(void);

void init_18b20()
{
        if((fd_18b20=open("/dev/DS18B20",O_RDWR | O_NDELAY | O_NOCTTY)) < 0)
        {
                printf("Open Device DS18B20 failed.\r\n");
                exit(1);
        }
        else
        {
                printf("Open Device DS18B20 successed.\r\n");
        }
}

float get_Tc(void)
{
        char buf[2];
        float result = 0;
        printf("r=%d\n",result);
        read(fd_18b20,buf,2);
        result = (float)buf[0];
        result /= 16;
        result += ((float)buf[1] * 16);
        if((result > 40)  | (result <-10))
         result = 25.2;
        printf("result=%f\n",result);
        return result;
}

main()
{
        float Tc;
        init_18b20();
        while(1)
        {
                Tc = get_Tc();
                printf("Tc Now is %.1f \n",Tc);
        }
}

