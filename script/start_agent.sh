#!/bin/sh

LD_LIBRARY_PATH=/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

AGENT_BIN=/usr/local/sbin

if [ -f /var/adm/snmpd.log ] 
then
	mv /var/adm/snmpd.log $AGENT_BID/snmpd.log.backup
else
	if [ -f /var/log/snmpd.log ]
	then
		mv /var/log/snmpd.log $AGENT_BIN/snmpd.log.backup
	fi
fi

#ulimit -n unlimited

echo "Start SmartECOAgent"
$AGENT_BIN/SmartECOAgent -c /usr/local/conf/snmpd.conf 
sleep 1

#echo "Start nasgated"
#$AGENT_BIN/nasgated -c /usr/local/conf/gated.conf 
#sleep 1

echo "Start agentChecker"
$AGENT_BIN/check.sh

if [ -f $AGENT_BIN/start_ora_agent.sh ]
then
    echo "Start Oracle Agent"
    $AGENT_BIN/start_ora_agent.sh 2>&1 > /dev/null &
fi

if [ -f $AGENT_BIN/start_unisql_agent.sh ]
then
    echo "Start Unisql Agent"
    $AGENT_BIN/start_unisql_agent.sh 2>&1 > /dev/null &
fi

