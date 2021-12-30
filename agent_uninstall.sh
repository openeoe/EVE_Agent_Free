#!/bin/sh
#

killSmartECOAgent ()
{
	/bin/ps -ef | grep SmartECOAgent | grep -v grep | grep " 1 " | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x

    if [ -f /usr/local/sbin/end_agent.sh ]
    then
        /usr/local/sbin/end_agent.sh
    fi
}

removeSmartECOAgentFile ()
{
    # sbin 昏力
    rm -rf /usr/local/sbin/SmartECOAgent
    rm -rf /usr/local/sbin/ICP
    rm -rf /usr/local/sbin/NASCenterMA
    rm -rf /usr/local/sbin/AM*
    rm -rf /usr/local/sbin/BM*
    rm -rf /usr/local/sbin/FD*
    rm -rf /usr/local/sbin/update*
    rm -rf /usr/local/sbin/rsync*
    rm -rf /usr/local/sbin/rbm*
    rm -rf /usr/local/sbin/check.sh
    rm -rf /usr/local/sbin/agentCheck.sh
    rm -rf /usr/local/sbin/start_agent.sh
    rm -rf /usr/local/sbin/end_agent.sh
    rm -rf /usr/local/sbin/status_agent.sh
    rm -rf /usr/local/sbin/register*
    rm -rf /usr/local/sbin/*.log
    rm -rf /usr/local/sbin/*.log.1
    rm -rf /usr/local/sbin/*.log.backup
    rm -rf /usr/local/sbin/*.log.backup.1
    rm -rf /usr/local/sbin/snmpget
    rm -rf /usr/local/sbin/snmpset
    rm -rf /usr/local/sbin/trap.exe
    rm -rf /usr/local/sbin/gettrapip.sh
    rm -rf /usr/local/sbin/settrapip.sh

    # lib 昏力
    rm -rf /usr/local/lib/libnetsnmp*
    rm -rf /usr/local/lib/libNAS*
    rm -rf /usr/local/lib/libpcap*
    rm -rf /usr/local/lib/libxml*

    # conf 昏力
    rm -rf /usr/local/conf/agentInfo.conf
    rm -rf /usr/local/conf/snmpd.conf
    rm -rf /usr/local/conf/ECOAgent_license.conf

    # mibs 昏力
    rm -rf /usr/local/mibs/NURI-MIB.mib

    # data 颇老 昏力
    rm -rf /var/net-snmp/*.data
}

removeSmartECOAgentRCFile ()
{
    if [ -f /etc/rc2.d/S999start_eco_agent ]
    then
        rm -rf /etc/rc2.d/S999start_eco_agent
    fi
}

removeSmartECOAgentCrontab ()
{
    OS=`uname -a | cut -d " " -f 1`
    CRONTAB_CP=crontab.copy
    CRONTAB_MOD=crontab.modify

    if [ -f $CRONTAB_MOD ]
    then
        rm -rf $CRONTAB_MOD
    fi

    crontab -l > $CRONTAB_CP

    while read line
    do
        MATCH=`echo $line | grep 'updateAgent.sh'`

        if [ "X$MATCH" = "X" ]
        then
            echo "$line" >> $CRONTAB_MOD
        fi
    done < $CRONTAB_CP

    crontab $CRONTAB_MOD

    rm -rf $CRONTAB_MOD
    rm -rf $CRONTAB_CP
}
killSmartECOAgent
removeSmartECOAgentFile
removeSmartECOAgentRCFile
removeSmartECOAgentCrontab
