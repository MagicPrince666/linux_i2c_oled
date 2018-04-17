#ifndef __I2C__OLED__H
#define __I2C__OLED__H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // sleep
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>



enum oled_sig{   //---command or data
oled_SIG_CMD,
oled_SIG_DAT
};


extern int g_fdOled;

//-------functions declaration----
void init_I2C_IOdata(void);
void free_I2C_IOdata(void);
void init_I2C_Slave(void);
void initOledTimer(void);
void sigHndlOledTimer(int signo);
void sendDatCmdoled(enum oled_sig datcmd,uint8_t val); // send data or command to I2C device
void sendCmdOled(uint8_t cmd);
void sendDatOled(uint8_t dat);
void initOledDefault(void); // open i2c device and set defaul parameters for OLED
void fillOledDat(uint8_t dat); // fill OLED GRAM with specified data
void drawOledAscii16x8(uint8_t start_row, uint8_t start_column,unsigned char c);
void push_Oled_Ascii32x18_Buff(const char* pstr, uint8_t nrow, uint8_t nstart);
void refresh_Oled_Ascii32x18_Buff(bool ref_all);
void drawOledStr16x8(uint8_t start_row, uint8_t start_column,const char* pstr);
void clearOledV(void); //clear OLED GRAM with vertical addressing mode, effective!
void setStartLine(int k);
void actOledScroll(void);
void deactOledScroll(void);
int intFcntlOp(int fd, int cmd, int type, off_t offset, int whence, off_t len);



#endif
