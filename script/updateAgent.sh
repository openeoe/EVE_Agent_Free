#!/bin/sh

LD_LIBRARY_PATH=/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

PATH=/usr/bin:/usr/sbin:/usr/local/bin:/usr/ccs/bin:$PATH
export PATH

MANAGERIP=103.22.253.10
RSYNC_EXE=/usr/local/sbin/rsync
HOSTNAME=`hostname`
LOCALIP=

OS=`uname -a | cut -d " " -f 1`
VERSION=`uname -r`

TARGET_DIR=/usr/local
AGENT_BIN_DIR=/usr/local/sbin

UPDATE_FILE_NAME=updateAgent.tar
UPDATE_FILE_PATH=/usr/local/sbin/update
UPDATE_FILE=$UPDATE_FILE_PATH/$UPDATE_FILE_NAME

ORACLE_UPDATE_FILE_NAME=updateOracleAgent.tar
ORACLE_UPDATE_FILE_PATH=/usr/local/sbin/update
ORACLE_UPDATE_FILE=$ORACLE_UPDATE_FILE_PATH/$ORACLE_UPDATE_FILE_NAME

UNISQL_UPDATE_FILE_NAME=updateUnisqlAgent.tar
UNISQL_UPDATE_FILE_PATH=/usr/local/sbin/update
UNISQL_UPDATE_FILE=$UNISQL_UPDATE_FILE_PATH/$UNISQL_UPDATE_FILE_NAME

AGENT_END_SCRIPT=$AGENT_BIN_DIR/end_agent.sh

AGENT_START_SCRIPT=$AGENT_BIN_DIR/start_agent.sh
ORACLE_AGENT_START_SCRIPT=$AGENT_BIN_DIR/start_ora_agent.sh
UNISQL_AGENT_START_SCRIPT=$AGENT_BIN_DIR/start_unisql_agent.sh

AGENT_SCRIPT=$AGENT_BIN_DIR/S999start_eco_agent
SNMPGET=$AGENT_BIN_DIR/snmpget
UPDATE_LOG=$AGENT_BIN_DIR/updateAgent.log

log_backup()
{
	if [ -f $UPDATE_LOG ]
	then
		fsize=`ls -al $UPDATE_LOG | awk '{print $5}'`
		if [ $fsize -gt 1048576 ]
		then
			mv $UPDATE_LOG $UPDATE_LOG.1
		fi
	fi
}

log ()
{
	DATE=`date '+%y/%m/%d %H:%M:%S'`
	if [ $UPDATE_LOG ]
	then
		echo "[$DATE] $*" >> $UPDATE_LOG
	fi

	log_backup
}

set_managerIp()
{
	community=`cat /usr/local/conf/snmpd.conf | grep community | awk '{print $2}'`
	if [ "X$community" = "X" ]
	then
		community=public
	fi
	if [  -f $SNMPGET ]
	then
		IPRESULT=`$SNMPGET -v 1 -c  $community 127.0.0.1 1.3.6.1.4.1.3204.1.3.36.1.4.0 | grep -v 'Timeout' | awk '{print $4}'`

		if [ "X$IPRESULT" = "X" ] 
		then
			log "[WARNING] set_managerIp::Manager IP does not setting in SmartECOAgent"
        elif [ $IPRESULT = "0.0.0.0" ]
        then
			log "[WARNING] set_managerIp::Manager IP does not setting in SmartECOAgent"
		else
			log "[INFO] set_managerIp::MANAGERIP=$IPRESULT"
			MANAGERIP=$IPRESULT
		fi
	else
		log "[WARNING] set_managerIp::does not exist $SNMPGET ManagerIP[$MANAGERIP]"
	fi

    return 0
}

rsync_updateFile ()
{
	set_managerIp
	log "[INFO] rsync_updateFile start"

	if [ ! -f $RSYNC_EXE ]
	then
		log "[ERROR] rsync_updateFile::does not exist $RSYNC_EXE "
		return 1
	fi
	log "[INFO] rsync_updateFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION $UPDATE_FILE_PATH | grep $UPDATE_FILE_NAME"
	RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION $UPDATE_FILE_PATH | grep $UPDATE_FILE_NAME`

    if [ $? -ne 0 ]
    then
        log "[ERROR] rsync_updateFile failed"
        log `$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION $UPDATE_FILE_PATH 2>&1`
        return 1
    fi        

	if [ "X$RSYNC_RESULT" = "X" ]
	then
		log "[INFO] rsync_updateFile::does not updated the $UPDATE_FILE"
		return 1
	fi

	while [ 2 -gt 1 ]
	do
		log "[INFO] rsync_updateFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION $UPDATE_FILE_PATH | grep $UPDATE_FILE_NAME"
		RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION $UPDATE_FILE_PATH | grep $UPDATE_FILE_NAME`
		if [ "X$RSYNC_RESULT" = "X" ]
		then
			LS=`ls -al $UPDATE_FILE`
			log "[INFO] rsync_updateFile::$LS"
			break;
		fi
		LS=`ls -al $UPDATE_FILE`
		log "[INFO] rsync_updateFile::$LS"
		sleep 20
	done

    return 0
}


rsync_putLogFile ()
{
    set_managerIp
    hname=`uname -n`
    while read ip host anotherhost;
    do
        case $ip in
            [0-9]*) 
            if [ $host = $hname ] || ( [ $anotherhost ] && [ $anotherhost = $hname ] );
            then
                echo $ip > iptmp.$$
                break;
            fi
        esac
    done < /etc/hosts
	
	if [ -f iptmp.$$ ]
	then
    	read ipAddr < iptmp.$$
    	rm iptmp.$$
    	LOCALIP=$ipAddr
	fi
    
    if [ "X$LOCALIP" = "X" ]
    then
        LOCALIP=` cat /etc/hosts | grep "$HOSTNAME" | grep -v "^#" | awk '{print $1}'`
    fi
	
	log "[INFO] rsync_putLogFile::LOCALIP=$LOCALIP"
    AGENT_LOG_FILE=/usr/local/sbin/eco_agent.log
    UPDATE_LOG_FILE=/usr/local/sbin/updateAgent.log
    SNMPD_LOG_BACKUP_FILE=/usr/local/sbin/snmpd.log.backup
    SNMPD_LOG_FILE=/var/adm/snmpd.log
    if [ -f /var/log/snmpd.log ]
    then
        SNMPD_LOG_FILE=/var/log/snmpd.log
    fi

    if [ -f $AGENT_LOG_FILE ]
    then
	    log "[INFO] rsync_putLogFile::$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $AGENT_LOG_FILE $MANAGERIP::LOG/eco_agent.log-$LOCALIP-$HOSTNAME"
	    log `$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $AGENT_LOG_FILE $MANAGERIP::LOG/eco_agent.log-$LOCALIP-$HOSTNAME`
    fi

    if [ -f $SNMPD_LOG_FILE ]
    then
	    log "[INFO] rsync_putLogFile::$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $SNMPD_LOG_FILE $MANAGERIP::LOG/snmp.log-$LOCALIP-$HOSTNAME"
	    log `$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $SNMPD_LOG_FILE $MANAGERIP::LOG/snmpd.log-$LOCALIP-$HOSTNAME`
    fi

    if [ -f $SNMPD_LOG_BACKUP_FILE ]
    then
	    log "[INFO] rsync_putLogFile::$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $SNMPD_LOG_BACKUP_FILE $MANAGERIP::LOG/snmp.log.backup-$LOCALIP-$HOSTNAME"
	    log `$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $SNMPD_LOG_BACKUP_FILE $MANAGERIP::LOG/snmpd.log.backup-$LOCALIP-$HOSTNAME`
    fi

    if [ -f $UPDATE_LOG_FILE ]
    then
	    log "[INFO] rsync_putLogFile::$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $UPDATE_LOG_FILE $MANAGERIP::LOG/updateAgent.log-$LOCALIP-$HOSTNAME"
	    log `$RSYNC_EXE -Cavuz --port=9101 --timeout=300 $UPDATE_LOG_FILE $MANAGERIP::LOG/updateAgent.log-$LOCALIP-$HOSTNAME`
    fi

    return 0
}

register_rc ()
{
    INSTALL_DIR=/usr/local

	chmod 755 $INSTALL_DIR/sbin/S999start_eco_agent
	log "[INFO] register_rc::cp $INSTALL_DIR/sbin/S999start_eco_agent /etc/rc2.d"
	cp $INSTALL_DIR/sbin/S999start_eco_agent /etc/rc2.d

    return 0
}

execute_updateAgent ()
{
	## check whether updatefile is existed or not
	if [ ! -f $UPDATE_FILE ]
	then
    	log "[ERROR] execute_updateAgent::does not exist $UPDATE_FILE"
    	return 1
	fi

	## check update tar file
	tar -tvf $UPDATE_FILE 

	if [ $? -ne 0 ]
	then
		log "[ERROR] execute_updateAgent::tar failed $UPDATE_FILE"
		return 1
	fi

	if [ -f /usr/local/sbin/end_agent.sh ]
	then
    	log "[INFO] execute_updateAgent::/usr/local/sbin/end_agent.sh"
    	/usr/local/sbin/end_agent.sh
	else
    	log "[ERROR] execute_updateAgent::can not found script file to stop SmartECOAgent "
    	return 1
	fi

	log "[INFO] execute_updateAgent::cp $UPDATE_FILE $TARGET_DIR/$UPDATE_FILE_NAME"
	cp $UPDATE_FILE $TARGET_DIR/$UPDATE_FILE_NAME
	log "[INFO] execute_updateAgent::cd $TARGET_DIR"
	cd $TARGET_DIR
	log "[INFO] execute_updateAgent::tar -xvf $UPDATE_FILE_NAME > /dev/null"
	tar -xvf $UPDATE_FILE_NAME > /dev/null
	log "[INFO] execute_updateAgent::rm -rf $UPDATE_FILE_NAME"
	rm -rf $UPDATE_FILE_NAME

    register_rc

	if [ -f /usr/local/sbin/start_agent.sh ]
	then
    	log "[INFO] execute_updateAgent::/usr/local/sbin/start_agent.sh"
    	/usr/local/sbin/start_agent.sh
	else
    	log "[ERROR] execute_updateAgent::can not found script file to start SmartECOAgent "
    	return 1
	fi

    return 0
}

chkStart ()
{
    PID=`/bin/ps -opid,comm -uroot | grep SmartECOAgent | grep -v grep`

    if [ "X$PID" = "X" ]
    then
        log "[INFO] chkStart::/usr/local/sbin/start_agent.sh"
        /usr/local/sbin/start_agent.sh
    else
        log "[INFO] chkStart::current running SmartECOAgent : $PID"
    fi
}

update_Agent ()
{
    rsync_updateFile
    RESULT=$? 
    if [ $RESULT -ne 0 ]
    then
        chkStart
        return 1
    fi
    execute_updateAgent
    chkStart
    return 0
}

rsync_updateOracleFile ()
{
	set_managerIp
	log "[INFO] rsync_updateOracleFile"

	if [ ! -f $RSYNC_EXE ]
	then
		log "[ERROR] rsync_updateOracleFile::does not exist $RSYNC_EXE "
		return 1
	fi

    SNMP_ORACLE_CONF=/usr/local/conf/snmpd-oracle.conf
    
    if [ ! -f $SNMP_ORACLE_CONF ]
    then
        log "[ERROR] rsync_updateOracleFile:: can not found $SNMP_ORACLE_CONF"
        return 1
    fi

    ORACLE_VERSION=`cat $SNMP_ORACLE_CONF | grep oraVersion | awk '{print $2}'`

    if [ "X$ORACLE_VERSION" = "X" ]
    then
        log "[ERROR] rsync_updateOracleFile::can not found Oracle Version"
        return 1
    fi

    if [ $ORACLE_VERSION = "806" ]
    then
        ORACLE_VERSION=80x
    fi

    ORACLE_VERSION=Oracle$ORACLE_VERSION

	log "[INFO] rsync_updateOracleFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$ORACLE_VERSION $ORACLE_UPDATE_FILE_PATH | grep $ORACLE_UPDATE_FILE_NAME"
	RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$ORACLE_VERSION $ORACLE_UPDATE_FILE_PATH | grep $ORACLE_UPDATE_FILE_NAME`

    if [ $? -ne 0 ]
    then
        log "[ERROR] rsync_updateOracleFile failed"
        log `$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$ORACLE_VERSION $ORACLE_UPDATE_FILE_PATH 2>&1`
        return 1
    fi        
	if [ "X$RSYNC_RESULT" = "X" ]
	then
		log "[INFO] rsync_updateOracleFile::does not updated the $ORACLE_UPDATE_FILE"
		return 1
	fi

	while [ 2 -gt 1 ]
	do
		log "[INFO] rsync_updateOracleFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$ORACLE_VERSION $ORACLE_UPDATE_FILE_PATH | grep $ORACLE_UPDATE_FILE_NAME"
		RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$ORACLE_VERSION $ORACLE_UPDATE_FILE_PATH | grep $ORACLE_UPDATE_FILE_NAME`
		if [ "X$RSYNC_RESULT" = "X" ]
		then
			LS=`ls -al $ORACLE_UPDATE_FILE`
			log "[INFO] rsync_updateOracleFile::$LS"
			break;
		fi
		LS=`ls -al $ORACLE_UPDATE_FILE`
		log "[INFO] rsync_updateOracleFile::$LS"
		sleep 20
	done

    return 0
}

execute_updateOracleAgent ()
{
	## check whether updatefile is existed or not
	if [ ! -f $ORACLE_UPDATE_FILE ]
	then
    	log "[WARNING] execute_updateOracleAgent::does not exist $ORACLE_UPDATE_FILE"
    	return 1
	fi

	## check update tar file
	tar -tvf $ORACLE_UPDATE_FILE

	if [ $? -ne 0 ]
	then
		log "[ERROR] execute_updateOracleAgent::tar failed $ORACLE_UPDATE_FILE"
		return 1
	fi

    /bin/ps -ef | grep scotty | grep -v grep | grep oracle | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x

	log "[INFO] execute_updateOracleAgent::cp $ORACLE_UPDATE_FILE $TARGET_DIR/$ORACLE_UPDATE_FILE_NAME"
	cp $ORACLE_UPDATE_FILE $TARGET_DIR/$ORACLE_UPDATE_FILE_NAME
	log "[INFO] execute_updateOracleAgent::cd $TARGET_DIR"
	cd $TARGET_DIR
	log "[INFO] execute_updateOracleAgent::tar -xvf $ORACLE_UPDATE_FILE_NAME > /dev/null"
	tar -xvf $ORACLE_UPDATE_FILE_NAME > /dev/null
	log "[INFO] execute_updateOracleAgent::rm -rf $ORACLE_UPDATE_FILE_NAME"
	rm -rf $ORACLE_UPDATE_FILE_NAME
    
    cd $TARGET_DIR/sbin
	if [ -f $ORACLE_AGENT_START_SCRIPT ]
	then
    	log "[INFO] execute_updateOracleAgent::$ORACLE_AGENT_START_SCRIPT"
    	/usr/local/sbin/start_ora_agent.sh 2>&1 > /dev/null &
	else
    	log "[ERROR] execute_updateOracleAgent::can not found file[/usr/local/sbin/start_ora_agent.sh]"
    	return 1
	fi
    return 0
}

chkStartOracle ()
{
    PID=`/bin/ps -opid,comm -uroot | grep scotty | grep oracle`

    if [ "X$PID" = "X" ]
    then
        cd /usr/local/sbin
        if [ -f /usr/local/sbin/start_ora_agent.sh ]
        then
            log "[INFO] chkStartOracle::/usr/local/sbin/start_ora_agent.sh "
            /usr/local/sbin/start_ora_agent.sh 2>&1 > /dev/null
        else
        	log "[ERROR] chkStartOracle::does not exist /usr/local/sbin/start_ora_agent.sh"
        fi
	else
        log "[INFO] chkStartOracle::current running OracleAgent : $PID"
    fi
}

update_OracleAgent ()
{
    rsync_updateOracleFile
    RESULT=$? 
    if [ $RESULT -ne 0 ]
    then
        if [ $RESULT -ne 2 ]
        then
            chkStartOracle
        fi
        return 1
    fi
    execute_updateOracleAgent
    chkStartOracle
    return 0
}

rsync_updateUnisqlFile ()
{
	set_managerIp
	log "[INFO] rsync_updateUnisqlFile"

	if [ ! -f $RSYNC_EXE ]
	then
		log "[ERROR] rsync_updateUnisqlFile::does not exist $RSYNC_EXE "
		return 1
	fi

    SNMP_UNISQL_CONF=/usr/local/conf/snmpd_unisql.conf
    
    if [ ! -f $SNMP_UNISQL_CONF ]
    then
        log "[ERROR] rsync_updateUnisqlFile::can not found $SNMP_UNISQL_CONF"
        return 2
    fi

    UNISQL_VERSION=Unisql

	log "[INFO] rsync_updateUnisqlFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$UNISQL_VERSION $UNISQL_UPDATE_FILE_PATH | grep $UNISQL_UPDATE_FILE_NAME"
	RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$UNISQL_VERSION $UNISQL_UPDATE_FILE_PATH | grep $UNISQL_UPDATE_FILE_NAME`

    if [ $? -ne 0 ]
    then
        log "[ERROR] rsync_updateOracleFile failed"
        log `$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$UNISQL_VERSION $UNISQL_UPDATE_FILE_PATH 2>&1`
        return 1
    fi        

	if [ "X$RSYNC_RESULT" = "X" ]
	then
		log "[INFO] rsync_updateUnisqlFile::does not updated the $UNISQL_UPDATE_FILE"
		return 1
	fi

	while [ 2 -gt 1 ]
	do
		log "[INFO] rsync_updateUnisqlFile::$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$UNISQL_VERSION $UNISQL_UPDATE_FILE_PATH | grep $UNISQL_UPDATE_FILE_NAME"
		RSYNC_RESULT=`$RSYNC_EXE -avz  --timeout=300 --port=9101 $MANAGERIP::$OS-$VERSION-$UNISQL_VERSION $UNISQL_UPDATE_FILE_PATH | grep $UNISQL_UPDATE_FILE_NAME`
		if [ "X$RSYNC_RESULT" = "X" ]
		then
			LS=`ls -al $UNISQL_UPDATE_FILE`
			log "[INFO] rsync_updateUnisqlFile::$LS"
			break;
		fi
		LS=`ls -al $UNISQL_UPDATE_FILE`
		log "[INFO] rsync_updateUnisqlFile::$LS"
		sleep 20
	done

    return 0
}

execute_updateUnisqlAgent ()
{
	## check whether updatefile is existed or not
	if [ ! -f $UNISQL_UPDATE_FILE ]
	then
    	log "[ERROR] execute_updateUnisqlAgent::does not exist $UNISQL_UPDATE_FILE"
    	return 1
	fi

	## check update tar file
	tar -tvf $UNISQL_UPDATE_FILE

	if [ $? -ne 0 ]
	then
		log "[ERROR] execute_updateUnisqlAgent::tar failed $UNISQL_UPDATE_FILE"
		return 1
	fi

    /bin/ps -ef | grep scotty | grep -v grep | grep unisql | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x

	log "[INFO] execute_updateUnisqlAgent::cp $UNISQL_UPDATE_FILE $TARGET_DIR/$UNISQL_UPDATE_FILE_NAME"
	cp $UNISQL_UPDATE_FILE $TARGET_DIR/$UNISQL_UPDATE_FILE_NAME
	log "[INFO] execute_updateUnisqlAgent::cd $TARGET_DIR"
	cd $TARGET_DIR
	log "[INFO] execute_updateUnisqlAgent::tar -xvf $UNISQL_UPDATE_FILE_NAME > /dev/null"
	tar -xvf $UNISQL_UPDATE_FILE_NAME > /dev/null
	log "[INFO] execute_updateUnisqlAgent::rm -rf $UNISQL_UPDATE_FILE_NAME"
	rm -rf $UNISQL_UPDATE_FILE_NAME
    
    cd $TARGET_DIR/sbin
	if [ -f /usr/local/sbin/start_unisql_agent.sh ]
	then
    	log "[INFO] execute_updateUnisqlAgent::/usr/local/sbin/start_unisql_agent.sh"
    	/usr/local/sbin/start_unisql_agent.sh 2>&1 > /dev/null &
	else
    	log "[ERROR] execute_updateUnisqlAgent::can not found file[/usr/local/sbin/start_unisql_agent.sh]"
    	return 1
	fi
    return 0
}

chkStartUnisql ()
{
    PID=`/bin/ps -opid,comm -uroot | grep scotty | grep unisql`
    if [ "X$PID" = "X" ]
    then
        cd /usr/local/sbin
        if [ -f /usr/local/sbin/start_unisql_agent.sh ]
        then
            log "[INFO] chkStartUnisql::/usr/local/sbin/start_unisql_agent.sh"
            /usr/local/sbin/start_unisql_agent.sh 2>&1 > /dev/null &
        else
        	log "[ERROR] chkStartOracle::dose not exists /usr/local/sbin/start_unisql_agent.sh"
        fi
	else
    	log "[INFO] chkStartOracle::current running UnisqlAgent : $PID"
    fi
}

update_UnisqlAgent ()
{
    rsync_updateUnisqlFile
    RESULT=$? 
    if [ $RESULT -ne 0 ]
    then
        if [ $RESULT -ne 2 ]
        then
            chkStartUnisql
        fi
        return 1
    fi
    execute_updateUnisqlAgent
    chkStartUnisql
    return 0
}

HOUR=`date '+%H'`
if [ $HOUR -eq 0 ]
then
    rsync_putLogFile
fi
update_Agent
update_OracleAgent
update_UnisqlAgent
