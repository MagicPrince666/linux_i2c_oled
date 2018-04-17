#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include "i2c_oled_128x64.h"
#include "msg_common.h"


int main(int argc,char** argv)
{
	char strtime[10]={0};

	char ascii_127[3];
	ascii_127[0]=129;
	ascii_127[1]=28;
	ascii_127[2]='\0';

	time_t timep;
	struct tm *p_tm;

	init_I2C_Slave();
	initOledDefault();

	initOledTimer();
	signal(SIGALRM,sigHndlOledTimer);

	clearOledV();
	usleep(300000);

	if((g_msg_id=createMsgQue(MSG_KEY_OLED_TEST))<0)
	{
		printf("create message queue fails!\n");
		exit(-1);
	}

	drawOledStr16x8(0,0,ascii_127);//test last ASCII symbol
	drawOledStr16x8(2,0,"-- Widora-NEO --");
	drawOledStr16x8(4,0,"-- Widora-NEO --");
	drawOledStr16x8(6,0,"-- Widora-NEO --");
	drawOledStr16x8(0,120,ascii_127);//test last ASCII symbol

  	while(1)
  	{
		timep=time(NULL);
		p_tm=localtime(&timep);
		strftime(strtime,8,"%H:%M:%S",p_tm);
		push_Oled_Ascii32x18_Buff(strtime,0,3);

		refresh_Oled_Ascii32x18_Buff(false);

		usleep(200000);
  	}

   	free_I2C_IOdata();
  	intFcntlOp(g_fdOled, F_SETLK, F_UNLCK, 0, SEEK_SET,10);

}
