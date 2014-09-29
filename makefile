################################################
#
#
#目录结构:
# core   		=>基础
#___lib			=>库文件
#___ROOT		=>主应用源文件
#   |
#   |...
#
################################################

#指定目标文件名
EXENAME = pthread-pool

#注释掉DEFINES，则编译RELEASE模式
DEFINES = -g

#指定目标文件所需的共享库
LIB_SO = pthread

#其他
#LIB_SO += sqlapi

#编译目录
ROOT = $(shell pwd)
SRC_DIR = $(ROOT)
EXE_DIR = $(ROOT)/bin
LIB_DIR = $(ROOT)/lib


#静态库
#STATIC_LIB = /usr/lib/libcrypt.a
#STATIC_LIB += $(LIB_DIR)/libsqlapi.a

#编译器
CC = g++

#目标文件
#EXES = $(EXE_DIR)/$(EXENAME)

INCLUDE += -I$(ROOT)/core -I$(ROOT)/test
#源文件
SRCS = $(wildcard *.cpp)
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')

#中间文件
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#编译优化选项
ifeq ($(origin DEFINES), undefined)
DEFINES = -O1  
OPTIONS = -s
endif

#编译选项：_LOGSCR - 日志信息输出到屏幕；_LOGFILE - 日志输出到文件；
CFLAGS = -Wall $(DEFINES) -D_LOGFILE
ifeq ($(origin OPTIONS), undefined)
CFLAGS += -D_LOGSCR
EXES = $(EXE_DIR)/$(EXENAME)d
else
CFLAGS += -D_RUNSERVICE
EXES = $(EXE_DIR)/$(EXENAME)
endif

#连接选项
LFLAGS = $(OPTIONS)
LFLAGS += -lm -ldl -rdynamic  -L$(LIB_DIR)
LFLAGS += $(foreach i, $(LIB_SO), -l$(i))

#规则
all:$(EXES)

$(EXES):$(OBJS)
	$(CC) -o $@ $(LFLAGS) $(OBJS) $(STATIC_LIB)

$(OBJS):%.o:%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE)

$(SRCS:.cpp=.o):$(INCS)

clean:
	-rm -f $(OBJS)
	rm -f $(EXES)

rebuild: clean all
