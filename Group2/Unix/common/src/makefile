GNU_CC = gcc
GNU_AR = ar

CC_FL = -Wall -c
AR_FL = -r

X_CC_FL = 
X_AR_FL = 

CC_D_FL =
CC_I_FL = -I. \
          -I../inc
CC_L_FL = 

GNU_CC_FL = $(CC_FL) $(X_CC_FL)
GNU_AR_FL = $(AR_FL) $(X_AR_FL)

GNU_COMPILE = $(GNU_CC) $(GNU_CC_FL) $(CC_D_FL) $(CC_I_FL) $(CC_L_FL)
GNU_ARCHIVE = $(GNU_AR) $(GNU_AR_FL) $(CC_L_FL)

COMPILE = $(GNU_COMPILE)   $?
ARCHIVE = $(GNU_ARCHIVE) $@ $? $(LNK_LIBS)

RMF = /bin/rm -f

BASENAME = NASC_FTP
LIBFILE  = lib$(BASENAME).a

.SUFFIXES : .o .c
.c.o : 
	$(RMF) $@;$(COMPILE)

#SRCFILES = NASCenter_FileTrans_Send_Util.c \
#           NASCenter_FileTrans_Recv_Util.c \
#           NASCenter_FileTrans.c

#OBJFILES = $(SRCFILES:.c=$(X_OBJ).o)

OBJFILES = $(C_OBJS)

C_OBJS = NASCenter_FileTrans_Send_Util.o \
		 NASCenter_FileTrans_Recv_Util.o \
		 NASCenter_FileTrans.o

SRCFILES = $(C_OBJS:%.o=%.c)

$(LIBFILE): $(OBJFILES)
	$(ARCHIVE)
	mv $(LIBFILE) ../libs

clean:
	$(RMF) $(LIBFILE) $(OBJFILES)
