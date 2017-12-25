EXEC	= oled
OBJS    = i2c_oled_128x64.o 
SRC     = i2c_oled_128x64.c

CROSS	= mipsel-openwrt-linux-uclibc-
CC	     = $(CROSS)gcc
STRIP	= $(CROSS)strip
CFLAGS	= -g -Wall
LDFLAGS += -lm -lpthread

all:  clean $(EXEC)
	
$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) 
	$(STRIP) $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	-rm -f $(EXEC) *.o
