#!/bin/sh
#
HOME=`pwd`

PATH=/usr/bin:/usr/local/bin:/usr/ccs/bin:/usr/contrib/bin:$HOME:$PATH
export PATH

# stop SmartECO Agent process
stop_ECOAgent () 
{
	echo "kill SmartECO Agent Process"
	/bin/ps -ef | grep SmartECOAgent | grep -v grep | grep " 1 " | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x
#	/bin/ps -ef | grep snmpd | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x
	/bin/ps -ef | grep updateAgent | grep -v grep | grep " 1 " | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x
}

# initialize SmartECO Agent configure file and library
initialize_ECOAgent ()
{
	echo "Initialize for SmartECO Agent"
	echo "Verifying psMonitorTable.data"

	if [ -f $PSMONITOR_DATA_FILE ]
	then
		./conv_ps -c -f $PSMONITOR_DATA_FILE
	fi

	echo "Verifying logMonRowDataFile.data"

	if [ -f $LOGMONROW_DATA_FILE ]
	then
		./conv_log -c -f $LOGMONROW_DATA_FILE
	fi

	rm -rf $INSTALL_DIR/lib/libnetsnmp*
	rm -rf $INSTALL_DIR/lib/libNAS*
}

# install SmartECO Agent
install_ECOAgent ()
{
	echo "INSTALL AGENT"

	if [ $? -ne $ZERO ]
	then
		echo "## Install_ECOAgent netstat failed !!!!!"
		exit
	fi
	
	echo "NET_STAT $NET_STAT"
	if [ "x$NET_STAT" = "x" ] ;
	then
		echo "other agent does not executing"
	else
		echo "## [ERROR]Already running other agent."
		echo "## [ERROR]Please find and kill"
		echo "## [ERROR]Restart install.sh"
		exit -1
	fi

	if [ -f /usr/local/conf/snmpd.conf ]
	then
		echo "==> copy old snmpd.conf to snmpd.conf.org"
		cp /usr/local/conf/snmpd.conf /usr/local/conf/snmpd.conf.org
	fi

	if [ ! -d $INSTALL_DIR/share/snmp/mibs ]
	then
		mkdir -p $INSTALL_DIR/share/snmp/mibs
		tar -xvf mibList.tar
		mv mibList/* $INSTALL_DIR/share/snmp/mibs
	else
		echo "## snmp mibs exist"
	fi
	
	if [ -f $HOME/SmartECOAgent_$OS.tar.gz ] 
	then
		INSTALL_FILE=SmartECOAgent_$OS.tar.gz
	else
		echo "## Does not exist $HOME/Sma---------------rtECOAgent_$OS.tar.gz"
		exit -1
	fi
	
	cp SmartECOAgent_$OS.tar.gz $INSTALL_DIR
	cd $INSTALL_DIR
	
	if [ -f $INSTALL_DIR/SmartECOAgent_$OS.tar ]
	then
		rm -rf SmartECOAgent_$OS.tar
	fi

	echo "gzip -d SmartECOAgent_$OS.tar.gz > /dev/null"
	gzip -d SmartECOAgent_$OS.tar.gz > /dev/null
	echo "tar -xvf SmartECOAgent_$OS.tar  > /dev/null"
	tar -xvf SmartECOAgent_$OS.tar  > /dev/null
	echo "rm -rf SmartECOAgent_$OS.tar > /dev/null"
	rm -rf SmartECOAgent_$OS.tar > /dev/null

	echo "chmod 777 /usr/local/sbin"
	chmod 777 /usr/local/sbin

	if [ -f /usr/local/conf/snmpd.conf.org ]
	then
		echo "==> restore old snmpd.conf"
		cp /usr/local/conf/snmpd.conf.org /usr/local/conf/snmpd.conf
	fi

	if [ -f $INSTALL_DIR/sbin/S999start_eco_agent ]
	then
		chmod 755 $INSTALL_DIR/sbin/S999start_eco_agent
		cp $INSTALL_DIR/sbin/S999start_eco_agent /etc/rc2.d
		cp $INSTALL_DIR/sbin/S999start_eco_agent /etc/rc3.d
		cp $INSTALL_DIR/sbin/S999start_eco_agent /etc/rc5.d

	else
		echo "## can not found start script $INSTALL_DIR/sbin/S999start_eco_agent"
		exit -1
	fi
	
	if [ $? -ne $ZERO ]
	then
		echo "## Install_ECOAgent failed !!!!!"
		exit
	fi
}

# print SmartECO Agent executing
printMsg_ECOAgent ()
{
	echo "################################"
	echo "install SmartECOAgent completed".
	echo "go $INSTALL_DIR/sbin"
	echo "execute start_agent.sh script"
	echo "execute agentCheck.sh script"
	echo "################################"
}

license_key()
{
	if [ -f /usr/local/conf/ECOAgent_license.lic ]
	then
		echo "old license file found "
	else
		while [ 2 -gt 1 ] ;
		do
			echo "###################################################"
			echo "###################################################"
			printf " Input The license Key : "
			read command
			if [ X$command != "X" ]
			then
				echo $command > /usr/local/conf/ECOAgent_license.lic
				break
			else
				echo " try again lincense "	
			fi
		done
	fi
}



start_ECOAgent()
{
	cd $INSTALL_DIR/sbin
    chmod +x *
	./start_agent.sh
	./status_agent.sh
}

register_crontab ()
{
	FILE=$HOME/crontab.copy
	CRONCONTENT_BODY="30 23,0,1,2 * * *  /usr/local/sbin/updateAgent.sh"


	crontab -l > $FILE

    if [ $? -ne 0 ]
    then
        echo "SmartECOAgent install failed : register updateAgent cron"
        echo "권한이 없다는 에러일 경우에는 담당자에게 crontab에 관하여 문의 바람."
        echo "/etc/cron.d/at.allow => /etc/cron.d/at.deny"
        echo "/etc/cron.d/cron.allow => /etc/cron.d/cron.deny"
        echo "install 후 원상 복구하여야 함"
        exit
    fi

	CHECK_UPDATE_CRON=`cat $FILE | grep updateAgent.sh`
	if [ "X$CHECK_UPDATE_CRON" = "X" ]
	then
		echo "$CRONCONTENT_BODY" >> $FILE
		crontab  $FILE

        if [ $? -ne 0 ]
        then
            echo "SmartECOAgent install failed : register updateAgent cron"
            echo "권한이 없다는 에러일 경우에는 담당자에게 crontab에 관하여 문의 바람."
            echo "/etc/cron.d/at.allow => /etc/cron.d/at.deny"
            echo "/etc/cron.d/cron.allow => /etc/cron.d/cron.deny"
            echo "install 후 원상 복구하여야 함"
            exit
        fi
	else
		echo "Already registered $CHECK_UPDATE_CRON"
	fi
	rm -rf $FILE
}


# install SmartECO Agent
install_agent ()
{
	echo "start SmartECOAgent"	
	stop_ECOAgent
	initialize_ECOAgent
	install_ECOAgent
#	printMsg_ECOAgent
	license_key
#	register_crontab
	start_ECOAgent
}

INSTALL_DIR=/usr/local
PSMONITOR_DATA_FILE=/var/net-snmp/psMonitorTable.data
LOGMONROW_DATA_FILE=/var/net-snmp/logMonRowDataFile.data
OS=`uname -a | cut -d " " -f 1`
ZERO=0

install_agent

cd $HOME
