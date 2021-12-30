#!/bin/sh

SNMPGET=/usr/local/sbin/snmpget

DEFAULT_PORT=9107

port=`cat /usr/local/conf/snmpd.conf | grep agentaddress | awk '{print $2}'`
if [ "X$port" = "X" ]
then
	DEFAULT_PORT=161
fi

community=`cat /usr/local/conf/snmpd.conf | grep community | awk '{print $2}'`
if [ "X$community" = "X" ]
then
	community=public
fi

TRAP_MANAGER_IP=`$SNMPGET -v 1 -c  $community 127.0.0.1:$DEFAULT_PORT 1.3.6.1.4.1.3204.1.3.36.1.4.0 | awk '{print $4}'`
echo "TRAP_MANAGER_IP : $TRAP_MANAGER_IP "

