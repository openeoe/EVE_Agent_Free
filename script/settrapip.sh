#!/bin/sh

if [ "x$1" = "x" ]
then
	echo "input the ManagerIP"
	echo "Usage: settrapip.sh managerip"
	exit
fi

DEFAULT_PORT=9107
port=`cat /usr/local/conf/snmpd.conf | grep agentaddress | awk '{print $2}'`
if [ "X$port" = "X" ]
then
	DEFAULT_PORT=161
fi

MANAGERIP=$1
LOCALIP=127.0.0.1:$DEFAULT_PORT

prog=`ps -ef | grep SmartECOAgent | grep -v grep | awk '{print $2}'`
if [ "x$prog" = "x" ]
then
    echo " SmartECOAgent does not running"
    echo " please execute the SmartECOAgent and try again"
    exit
fi

community=`cat /usr/local/conf/snmpd.conf | grep community | awk '{print $2}'`
if [ "X$community" = "X" ]
then
	community=public
fi

echo "LOCALIP[$LOCALIP] MANAGERIP[$MANAGERIP]"
/usr/local/sbin/snmpset -v 1 -c $community $LOCALIP 1.3.6.1.4.1.3204.1.3.36.1.4.0 a $MANAGERIP
/usr/local/sbin/snmpset -v 1 -c $community $LOCALIP 1.3.6.1.4.1.3204.1.3.36.1.5.0 i 162
