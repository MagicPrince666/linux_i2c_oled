/*--------------------------------------
Common head for message queue IPC
--------------------------------------*/
#ifndef __MSG_COMMON_H__
#define __MSG_COMMON_H__

#include <string.h> //strncpy()
#include <stdio.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <errno.h> //-EAGAIN

#define MSG_BUFSIZE  64
//---- OLED Display Msg Queue ---
#define MSG_KEY_OLED_TEST 5678 //--- msg queue identical key
#define MSG_TYPE_TING 1  //---msg from ting
#define MSG_TYPE_CC1101 2 //---msg from cc1101
#define MSG_TYPE_WAIT_CC1101 3  //---msg:wait for cc1101 
//----  MOTOR CONTROL Msg Queue -----
#define MSG_KEY_MOTOR 5679 //--msg queue id. key for motor control
#define MSG_TYPE_MOTOR_PWM_WIDTH 1 //--msg type 1
//----  STEERING GEAR Msg Queue -----
#define MSG_KEY_SG 5680 //---msg queue id. key for steering gear 
#define MSG_TYPE_SG_PWM_WIDTH 4 // WARNING !!! msg type also MUST be globally identical

#define TIMER_TV_SEC 1 //10; //(s)  timer routine interval for CC1101 and TING_RX to receive IPC Msg! 
#define TIMER_TV_USEC 0 //(us)  timer routine interval
#define KEEP_RSSI_TIME 10  //(s) keep g_strTingBuf[] and g_strCC1101Buf[] with last received RSSI value if they've not been refreshed within the time limit.
//and fill g_strTingBuf[] and g_strCC1101Buf[] with '-----' when it exceeds the limit.

//----msg_conf.msg_qbytes=sizeof(g_msg_data)*MSG_DATA_BUF_NUM;
#define MSG_DATA_BUF_NUM 64  //
struct g_st_msg
{
	long int msg_type;
	char text[MSG_BUFSIZE];
};

extern int g_msg_id;

void initOledTimer(void);
void sigHndlOledTimer(int signo);
int createMsgQue(key_t key);

#endif
