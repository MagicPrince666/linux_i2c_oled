#include "msg_common.h"
#include "i2c_oled_128x64.h"

static struct g_st_msg g_msg_data; //message data
//static long int msg_type=0; //message type
static  struct msqid_ds msg_conf;//msg configuration set

int g_msg_id=-1;

//---- timer ----
struct itimerval g_tmval,g_otmval;
//timeval of when last msg received
struct timeval tm_last_msg= \
{
  .tv_sec=0,
  .tv_usec=0,
};
struct timeval tm_now;

//----  info string for CC1101 & Ting
char g_strCC1101Buf[]="CC1101:---------";//---16 characters for one line of oled
char g_strTingBuf[]="T:--------------"; 

/*-------------------------------------------------------
 create message queue and return message queue identifier
-------------------------------------------------------*/
int createMsgQue(key_t key)
{
  int msg_id=-1;

  //----- create message IPC ------
  msg_id=msgget(key,0666 | IPC_CREAT);
  if(msg_id<0)
  {
        perror("msgget error");
        return msg_id;
   }

  //----!!!! Minimize msg queue size to eliminate accumulation of msg -------
  if(msgctl(msg_id,IPC_STAT,&msg_conf)==0)
  {
        msg_conf.msg_qbytes=sizeof(g_msg_data)*MSG_DATA_BUF_NUM;//!!!!-depend on how many clients will open and send data to it simutaneouly !!!!
        if(msgctl(msg_id,IPC_SET,&msg_conf)==0)
                printf("msgctl: reset msg_conf to allocate  %d messages space in the queue succeed!\n",MSG_DATA_BUF_NUM);
  }
  else
        printf("msgctl: reset msg_conf to allocate %d messages in the queue fails!\n", MSG_DATA_BUF_NUM);

  return msg_id;
}

/*-------------------------------------------------------
get total number of message type in a Msg Queue
return
	>=0   Total numer of message
	-1    Msgctl()  error
-------------------------------------------------------*/
static int getMsgNum(int msg_id)
{
   static struct msqid_ds ds_buf;

   if(msgctl(msg_id,IPC_STAT,&ds_buf)==0)
   {
	return ds_buf.msg_qnum;
   }

   else
   {
	perror("msgctl() to get total number of messages:");
	return -1;
   }

}

/*-------------------------------------------------------
 receive data in message queue with specified message type
 return:
	>0   Number of bytes copied into message buffer
	-1   On error
-------------------------------------------------------*/
static int recvMsgQue(int msg_id,long msg_type)
{
   int msg_ret=-1;

   //-----get IPC message-----
   msg_ret=msgrcv(msg_id,(void *)&g_msg_data,MSG_BUFSIZE,msg_type,IPC_NOWAIT);  // return number of bytes copied into msg buf
   if(msg_ret == EAGAIN)
   {
//	printf("No message with specified type now.\n");
	return msg_ret;
   }
   if(msg_ret<0) //??? how about =0?
   {
        //perror("msgrcv");
        //printf("Receive IPC message fails!\n");
        return msg_ret;
   }
   else
   {
        printf("IPC Message received:%s\n",g_msg_data.text);
        return msg_ret;
   }

}


/*-------------------------------------------------------
 send data to  message queue with specified message type
 return 0 if succeed.
-------------------------------------------------------*/
static int sendMsgQue(int msg_id,long msg_type, char *data)
{
    int msg_ret=-1;

    g_msg_data.msg_type=msg_type;
    strncpy(g_msg_data.text,data,sizeof(g_msg_data.text));
    msg_ret=msgsnd(msg_id,(void *)&g_msg_data,MSG_BUFSIZE,IPC_NOWAIT); //non-blocking
    if(msg_ret == EAGAIN)
    {
	printf("Space not available now.\n");
	return msg_ret;
    }
    else if(msg_ret != 0 ) 
    {
         perror("msgsnd failed");
	 return msg_ret;
    }
    return msg_ret;
}



/*-----------------------------
set timer for routine operation
500ms
-------------------------------*/
void initOledTimer(void)
{
   g_tmval.it_value.tv_sec=TIMER_TV_SEC;
   g_tmval.it_value.tv_usec=TIMER_TV_USEC; 
   g_tmval.it_interval.tv_sec=TIMER_TV_SEC;
   g_tmval.it_interval.tv_usec=TIMER_TV_USEC;

   setitimer(ITIMER_REAL,&g_tmval,&g_otmval);
}


/*-----------------------------------------------------
    routine process for Timer in OLED program
-------------------------------------------------------*/
void sigHndlOledTimer(int signo)
{
   int msg_ret;

   //------- receive mssage queue from Ting ------
   msg_ret=recvMsgQue(g_msg_id,MSG_TYPE_TING);
   if(msg_ret >0)
   {

    sprintf(g_strTingBuf,"T:%s    ",g_msg_data.text);
	//--- push to Oled frame buff
	push_Oled_Ascii32x18_Buff(g_strTingBuf,2,0);
	//--- record time stamp
	gettimeofday(&tm_last_msg,NULL);
    }
    else if(msg_ret != EAGAIN) //--bypass EAGAIN
    {
	gettimeofday(&tm_now,NULL);
	if((tm_now.tv_sec-tm_last_msg.tv_sec)>KEEP_RSSI_TIME)
	        sprintf(g_strTingBuf,"Ting: --------- ");
    }
    while(msg_ret > 0 ) //---- read out all remaining msg from Ting 
    {
         msg_ret=recvMsgQue(g_msg_id,MSG_TYPE_TING); 
     }

    //------- receive mssage queue from CC1101 ------
    msg_ret=recvMsgQue(g_msg_id,MSG_TYPE_CC1101);  
    if(msg_ret > 0 ){
       	//---put to CC1101 buffer ---
        sprintf(g_strCC1101Buf,"CC1101: %s  ",g_msg_data.text);
	//--- push to Oled frame buff ---
	push_Oled_Ascii32x18_Buff(g_strCC1101Buf,1,0);
    }
    else if(msg_ret != EAGAIN)
          sprintf(g_strCC1101Buf,"CC1101: ------- ");

    while(msg_ret > 0 ) //---- read out all remaining msg from CC1101
    {
         msg_ret=recvMsgQue(g_msg_id,MSG_TYPE_CC1101); 
     }

    //--- send msg to CC1101 to let it send msg ----,for CC1101 sndmsg is much faster than Ting.
    //---- Only if there is enought space left for other applications, in case CC1101_rx is down.
    if(getMsgNum(g_msg_id)<MSG_DATA_BUF_NUM/2)
    	sendMsgQue(g_msg_id,MSG_TYPE_WAIT_CC1101,"wait cc1101");

}