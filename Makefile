CPP = mipsel-openwrt-linux-uclibc-gcc

TARGET	= oled 

DIR		= . 
INC		= -I. 
CFLAGS	= -g -Wall

OBJPATH	= ./obj

FILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.c))

OBJS	= $(patsubst %.c,%.o,$(FILES))

all:$(OBJS) $(TARGET)

$(OBJS):%.o:%.c
	$(CPP) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $< 

$(TARGET):$(OBJPATH)
	$(CPP) -o $@ $(OBJPATH)/*.o

$(OBJPATH):
	mkdir -p $(OBJPATH)

clean:
	-rm $(OBJPATH)/*.o
	-rm $(TARGET)