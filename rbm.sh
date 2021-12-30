#!/bin/sh
set -x
PATH=/usr/local/bin:$PATH
export PATH

#Variable Definitions
SOURCE=Group1/RBM/src/rbm.c
TARGET=rbm

LIB1=libnetsnmpmibs
LIB2=libnetsnmpagent
LIB3=libnetsnmphelpers
LIB4=libnetsnmp

#Some common PATH definitions
NET_SNMP_PATH=Group1/net-snmp-5.8
INC_PATH="-IGroup1/RBM/inc"
TARGET_PATH=/usr/local/sbin
LIB_PATH=/usr/local/lib

LIB_HP="-lnm"
LIB_SOL="-lkstat -lsocket -lnsl -ladm -lpcap -lNASC_FTP -lkvm"
LIB_UNW="-lsocket -lnsl -lgen -lthread -lelf"
#LIB_LNX="-lrpm -lrpmio -lrpmdb -lpopt -lpthread -lssl -lpcap -lNASC_FTP"
LIB_LNX="-lpthread -lpcap -lNASC_FTP"
#LIB_TRU="-lpcap -lcrypto -lpthread -lNASC_FTP"
LIB_TRU="-lpcap -lpthread -lNASC_FTP"

/usr/bin/gcc -g -o $TARGET_PATH/$TARGET $SOURCE $INC_PATH -I$NET_SNMP_PATH/include -I$NET_SNMP_PATH/include/net-snmp/library $LIB_PATH/$LIB3.so $LIB_PATH/$LIB2.so $LIB_PATH/$LIB4.so $LIB_PATH/$LIB1.so $LIB_LNX

