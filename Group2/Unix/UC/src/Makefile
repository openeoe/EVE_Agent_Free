INCPATH=../inc/
CC=/usr/local/bin/gcc 
CFLAGS= -I../inc/  -Wall -g  -c  
FLAGS=-L /usr/local/lib  -lnetsnmp -lnetsnmpagent -lnetsnmphelpers -lnetsnmpmibs -lrt -lpopt -lsocket -lkstat -ladm -lelf -ldl -lpcap -lnsl -lpthread -lNASC_FTP

BINPATH=../src/


OBJS=NASCenter_UM_List.o\
  NASCenter_UM_Firewall.o
	
HEADERS=$(INCPATH)NASCenter_UM_Firewall.h $(INCPATH)NASCenter_UM_List.h $(INCPATH)ctypes.h $(INCPATH)NASCenter_Errors.h $(INCPATH)allowFTPTable.h $(INCPATH)allowPOP3Table.h $(INCPATH)allowSMTPTable.h  $(INCPATH)allowTelnetTable.h


FIREWALL:	$(OBJS)
	$(CC)  $(OBJS)  -o   $(BINPATH)firewall_other -lpthread $(FLAGS)


NASCenter_UM_List.o	:	NASCenter_UM_List.c $(HEADERS)
		$(CC)  $(CFLAGS) NASCenter_UM_List.c  

NASCenter_UM_Firewall.o	:	NASCenter_UM_Firewall.c $(HEADERS)
		$(CC)  $(CFLAGS) NASCenter_UM_Firewall.c  


clean	:	
		$(RM)  NASCenter_UM_Firewall.o err
		$(RM)  NASCenter_UM_List.o 
		$(RM)  firewall_other
