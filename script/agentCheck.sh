#!/bin/sh

ManagerIp=103.22.254.6

checkProcess () 
{
	UNIX95= ps -opcpu,vsz,etime,time,pid,comm -u$uid |
	( 
		retval=0
		read line
		while  read cpu vsz etime time pid comm; do
			case $comm in
				*$progName* ) 
					if [ $retval -gt 0 ]
					then
						break
					fi
					log "`date '+%Y/%m/%d %H:%M:%S'` : $comm : $cpu / $vsz"
					retval=`expr $retval + 1`
					cpu=`echo $cpu ' * 100 / 100' | bc`
			if [ $cpuThreshold -ne 0 ] && [ $cpuThreshold -lt $cpu ];
			then
				log "WARNING !!! : [$progName] CPU Threshold[$cpuThreshold] value=[$cpu]"
				sendAlert $cpu $vsz "WARNING !!! : [$progName] CPU Threshold[$cpuThreshold] value=[$cpu]"
				dumpAndKill $cpu $vsz $pid
			fi
			if [ $memoryThreshold -ne 0 ] && [ $memoryThreshold -lt $vsz ];
			then
				log "WARNING !!! : [$progName] Memory Threshold[$memoryThreshold] value=[$vsz]"
				sendAlert $cpu $vsz "WARNING !!! : [$progName] Memory Threshold[$memoryThreshold] value=[$vsz]"

				dumpAndKill $cpu $vsz $pid
			fi
			esac
		done  
		return $retval
	)
	return $?
}

checkProcess_linux () 
{
	UNIX95= ps -opcpu,rss,etime,time,pid,comm -u$uid |
	( 
		retval=0
		read line
		while  read cpu rss etime time pid comm; do
			case $comm in
				*$progName* ) 
					if [ $retval -gt 0 ]
					then
						break
					fi
					log "`date '+%Y/%m/%d %H:%M:%S'` : $comm : $cpu / $rss"
					retval=`expr $retval + 1`
					cpu=`echo $cpu ' * 100 / 100' | bc`
			if [ $cpuThreshold -ne 0 ] && [ $cpuThreshold -lt $cpu ];
			then
				log "WARNING !!! : [$progName] CPU Threshold[$cpuThreshold] value=[$cpu]"
				sendAlert $cpu $rss "WARNING !!! : [$progName] CPU Threshold[$cpuThreshold] value=[$cpu]"
				dumpAndKill $cpu $rss $pid
			fi
			if [ $memoryThreshold -ne 0 ] && [ $memoryThreshold -lt $rss ];
			then
				log "WARNING !!! : [$progName] Memory Threshold[$memoryThreshold] value=[$rss]"
				sendAlert $cpu $rss "WARNING !!! : [$progName] Memory Threshold[$memoryThreshold] value=[$rss]"

				dumpAndKill $cpu $rss $pid
			fi
			esac
		done  
		return $retval
	)
	return $?
}

dumpAndKill()
{
	if [ $processKill = "yes" ];
	then
		dump $1 $2
		killProcess $3
		if [ $progName = "SmartECOAgent" ]
		then
			/usr/local/sbin/start_agent.sh
		fi
	fi
}

log_backup ()
{
	if [ -f $filename ]
	then
		fsize=`ls -al $filename | awk '{print $5}'`
		if [ $fsize -gt $maxfilesize ]
		then
			mv $filename $filename.1
		fi
	fi
}

log ()
{
	if [ $filename ];
	then
		echo $* >> $filename 
	fi
	
	log_backup
}

dump()
{
	log "       Date : `date`"
	log "      pName : $progName" 
	log "     intval : $interval" 
	log "     pCount : $progCnt" 
	log "        cpu : $1 / $cpuThreshold" 
	log "     memory : $2 / $memoryThreshold" 
}

killProcess()
{
	kill -9 $1
}

setSendAlertIp ()
{

	MANAGERIP=`$SNMPGET -v 1 -c  $community 127.0.0.1 1.3.6.1.4.1.3204.1.3.36.1.4.0 | awk '{print $4}'`

	if [ "X$MANAGERIP" = "X" ]
	then
		log "Manager Ip address does not setting in SmartECOAgent"
	elif [ $MANAGERIP = "0.0.0.0" ] 
	then
		log "Manager Ip address does not setting in SmartECOAgent"
	else
		ManagerIp=$MANAGERIP
	fi
}

sendAlert ()
{

	setSendAlertIp

	if [ $processKill = "yes" ];
	then
		sendAlert_kill $*
	else
		sendAlert_waring $*
	fi
}

sendAlert_waring()
{
	CurrentCpu=$1;shift;
	CurrentMemory=$1;shift;

	CurrentCpu=`echo $CurrentCpu ' * 100 / 100' | bc`

	log $TrapExe -v 1 -c $community $ManagerIp $TrapOid $ipAddr 6 0 $UpTime \
		$CurrentProcessNameOid string $progName \
		$CurrentCpuOid integer $CurrentCpu \
		$CurrentMemoryOid integer $CurrentMemory \
		$TrapMessageOid string "$*"

	$TrapExe -v 1 -c $community $ManagerIp $TrapOid $ipAddr 6 0 $UpTime \
		$CurrentProcessNameOid string $progName \
		$CurrentCpuOid integer $CurrentCpu \
		$CurrentMemoryOid integer $CurrentMemory \
		$TrapMessageOid string "$*"
}

sendAlert_kill()
{
	CurrentCpu=$1;shift;
	CurrentMemory=$1;shift;

	CurrentCpu=`echo $CurrentCpu ' * 100 / 100' | bc`
	AlertMessage="$progName DOWN CPU:$CurrentCpu MEMORY;$CurrentMemory"

    	log "$TrapExe  -v 2c -c $community $ManagerIp  $UpTime $ProcessTrapOid \
        	$ProcessNameOid string $progName \
        	$ProcessFilterOid string $Filter \
        	$ProcessAlertMessageOid string $AlertMessage "

	$TrapExe  -v 2c -c $community $ManagerIp  $UpTime $ProcessTrapOid \
		$ProcessNameOid string $progName \
		$ProcessFilterOid string $Filter \
		$ProcessAlertMessageOid string "$AlertMessage"
}

sendAlert_up()
{

	setSendAlertIp

	CurrentCpu=$1;shift;
	CurrentMemory=$1;shift;

	CurrentCpu=`echo $CurrentCpu ' * 100 / 100' | bc`
	AlertMessage="$progName DOWN CPU:$CurrentCpu MEMORY;$CurrentMemory"

    	log "$TrapExe  -v 2c -c $community $ManagerIp  $UpTime $ProcessTrapOid \
        	$ProcessNameOid string $progName \
        	$ProcessFilterOid string up \
        	$ProcessAlertMessageOid string $AlertMessage "

	$TrapExe  -v 2c -c $community $ManagerIp  $UpTime $ProcessTrapOid \
		$ProcessNameOid string $progName \
		$ProcessFilterOid string up \
		$ProcessAlertMessageOid string "$AlertMessage"
}

backup()
{
	if [ $filename ];
	then
		bfile=$filename.`date '+%d_%H_%M_%S'`.msg
		mv $filename $bfile
        mailx -s "[$ipAddr] ECO Log Mail : $progName" "ecoagent@[$ManagerIp]" < $bfile
        rm -f $bfile
	fi
}

Main ()
{
	sendAlert_up 0 0 "WARNING !!! : [$progName] process UP"
	while checkProcess_linux 
		[ $? -gt $progCnt ]
	do
		sleep $interval
		sumTime=`expr $sumTime + $interval`
		if [ $backupTime -ne 0 ] && [ $backupTime -lt `expr $sumTime / 60` ];
		then
			backup
			sumTime=0
		fi
	done

	log "WARNING !!! : [$progName] process DOWN"
	sendAlert_kill 0 0 "WARNING !!! : [$progName] process DOWN"

	dump 0 0

	exit 0
}

usage ()
{
	cat <<EOF
agentCheck.exe [-hciupmfak] processName
	h : print this page
	c : process count (default 0)
	i : interval (default 60 sec)
	u : user id (default root)
	p : Processor Threshold ( 0 is not check)
	m : Memory Threshold ( 0 is not check)
	f : lof file
	b : Backup Time (default 0 min)
	k : kill process 
EOF
}

#
# Start
#

if [ $# -lt 1 ]
then
	usage
	exit 1
fi

# Option 초기화
# Argument를 progName으로 할당
# progName : process Name 반드시 필요
# interval : default 60 sec
# uid : userid default root
# progCnt : program Count (default 0)
interval=60
uid=0
progCnt=0
cpuThreshold=0
memoryThreshold=0
Filter=down
AlertMessage=
filename= 
snmplogfile=
backupTime=0
sumTime=0
ipAddr=
maxfilesize=1
processKill="no"
SNMPGET=/usr/local/sbin/snmpget

DirName=/usr/local/sbin
TrapExe=$DirName/trap.exe
UpTime=0

NuriOid=1.3.6.1.4.1.3204
ProcessTrapOid=$NuriOid.1.3.35.3
ProcessNameOid=$NuriOid.1.3.35.3.1.2
ProcessFilterOid=$NuriOid.1.3.35.3.1.6
ProcessAlertMessageOid=$NuriOid.1.3.35.3.1.11

TrapOid=$NuriOid.1.3.35.3.1.1.0                 # psMonitorIndex.0
CurrentProcessNameOid=$NuriOid.1.3.33.1.1.4     # psRunningName
CurrentCpuOid=$NuriOid.1.3.33.1.1.15            # psRunningPercentCPU
CurrentMemoryOid=$NuriOid.1.3.33.1.1.8          # psRunningSize
TrapMessageOid=$NuriOid.1.3.35.4.1.11           # trapMessage

community=`cat /usr/local/conf/snmpd.conf | grep community | awk '{print $2}'`
if [ "X$community" = "X" ]
then
	community=public
fi

while getopts hi:u:c:p:m:f:b:s:n:k opt 
do
	case $opt in
		i) 		interval=$OPTARG;;
		u)		uid=$OPTARG;;
		c)		progCnt=$OPTARG;;
		p)		cpuThreshold=$OPTARG;;
		m)		memoryThreshold=$OPTARG;;
		f)		filename=$OPTARG;;
		b)		backupTime=$OPTARG;;
		k)		processKill="yes";;
		n)		progName=$OPTARG;;
		s)		maxfilesize=$OPTARG;;
		?)		usage;
				exit 2;;
	esac
done

if [ -f /var/adm/snmpd.log ]
then
	snmplogfile=/var/adm/snmpd.log
else
	if [ -f /var/log/snmpd.log ]
	then
		snmplogfile=/var/log/snmpd.log
	fi
fi

#progName=$1

hname=`uname -n`

maxfilesize=`expr $maxfilesize \* 1024 \* 1024`

log "##########start agentCheck.sh##############"
log "INTERVAL=$interval"
log "UID=$uid"
log "PROGCNT=$progCnt"
log "CPUTHREASHOLD=$cpuThreshold"
log "MEMORYTHRESHOLD=$memoryThreshold"
log "FILENAME=$filename"
log "BACKUPTIME=$backupTime"
log "PROCESSKILL=$processKill"
log "MAXFILESIZE=$maxfilesize"
log "PROGNAME=$progName"
log "###########################################"

while read ip host anotherhost antoherhost1;
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
fi

if [ "X$ipAddr" = "X" ]
then
	ipAddr=`cat /etc/hosts | grep "$hname" | grep -v "^#" | awk '{print $1}'`
fi

if [ $ipAddr ];
then
    Main
else
    log "get Ip Address fail !!"
    exit 3
fi

