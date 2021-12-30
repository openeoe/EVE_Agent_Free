#!/bin/sh

set -x
# Setting PATH
PATH=/usr/local/bin:/usr/ccs/bin:/usr/contrib/bin:$PATH
export PATH

#Constants
ZERO=0

gcc -v
if [ $? -ne $ZERO ]
	then
	echo "gcc not found. Please set gcc path in PATH variable"
	exit
fi


RELEASE_VERSION=FINAL_RELEASE
rm -rf GkesBuild
rm -rf /usr/local/conf/snmpd.conf
rm -rf /usr/local/lib/libnetsnmp*
rm -rf /usr/local/sbin/Backup*
rm -rf /usr/local/sbin/JobSchedule*
rm -rf /usr/local/sbin/ICP
rm -rf /usr/local/sbin/FD_*
rm -rf /usr/local/sbin/AM_*
rm -rf /usr/local/sbin/firewall

# Setting PATH
LD_LIBRARY_PATH=/usr/lib:/usr/ccs/lib:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH


#Some common PATH definitions
NURI_UNIX_SRC_PATH=Group1
NET_SNMP_PATH=Group1/net-snmp-5.8

NURI_UNIX_SRC_G2_PATH=Group2/Unix/common/src
NURI_UNIX_INC_G2_PATH=Group2/Unix/common/inc

NET_SNMP_MIB_MODULE_PATH=Group1/net-snmp-5.8/agent/mibgroup/nuri
MA_PATH="Group1/MasterAgent"
BUILD_HOME=`pwd`
#PATH definitions End



TARGET_PATH=/usr/local/sbin/
#Group2 Module............

NURI_UNIX_G2_PATH=Group2/Unix
NURI_G2_AGENT_PATH=Group1/G2Configure

#Modules to be compiled
MIB_MODULES_G1=" nuri/agentInfo nuri/sysStatisticsTable nuri/trapTable nuri/collectDataTable nuri/logMonRowTable  nuri/logMonColumnTable nuri/subAgentConfig nuri/psMonitorTable  nuri/monitorPolicyTable nuri/netApplicationMonitoringTable nuri/transactionTable"

MIB_MODULES_G2="nuri/appMeteringTable nuri/jobProcessMasterTable nuri/JobProcessDetailTable nuri/backupMasterTable nuri/backupDetailTable nuri/allowTelnetTable nuri/allowFTPTable nuri/allowSMTPTable nuri/allowPOP3Table nuri/icpTable nuri/userAccessControlTable nuri/userGroupTable"
#End of Modules

#Creating Gkes Directory under  NET-SNMP
mkdir  -p $NET_SNMP_MIB_MODULE_PATH
#Gkes Directory Created


#Copying MIB modules

#AgentInfo Files
cp $NURI_UNIX_SRC_PATH/Common/src/agentInfo.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/Common/inc/agentInfo.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/appMeteringTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/appMeteringTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/backupMasterTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/backupMasterTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/backupDetailTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/backupDetailTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/jobProcessMasterTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/jobProcessMasterTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/JobProcessDetailTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/JobProcessDetailTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/allowFTPTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/allowFTPTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/allowTelnetTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/allowTelnetTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/allowSMTPTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/allowSMTPTable.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/allowPOP3Table.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/allowPOP3Table.h $NET_SNMP_MIB_MODULE_PATH

cp $NURI_G2_AGENT_PATH/src/icpTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_G2_AGENT_PATH/inc/icpTable.h $NET_SNMP_MIB_MODULE_PATH

#SubAgent Files
cp $NURI_UNIX_SRC_PATH/Common/src/subAgentConfig.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/Common/inc/subAgentConfig.h $NET_SNMP_MIB_MODULE_PATH

#ProcMon Files
cp $NURI_UNIX_SRC_PATH/ProcMon/src/psMonitorTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/ProcMon/inc/psMonitorTable.h $NET_SNMP_MIB_MODULE_PATH

#LogMon Files
cp $NURI_UNIX_SRC_PATH/LogMon/src/logMonRowTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/inc/logMonRowTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/src/logMonColumnTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/inc/logMonColumnTable.h $NET_SNMP_MIB_MODULE_PATH

#sysStatisticsTable files
cp $NURI_UNIX_SRC_PATH/sysStats/inc/sysStatisticsTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/sysStats/src/sysStatisticsTable.c $NET_SNMP_MIB_MODULE_PATH
#RTCM Files
cp $NURI_UNIX_SRC_PATH/RTCM/src/collectDataTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RTCM/inc/collectDataTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RTCM/src/trapTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RTCM/inc/trapTable.h $NET_SNMP_MIB_MODULE_PATH

#RBM Files
cp $NURI_UNIX_SRC_PATH/RBM/src/monitorPolicyTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RBM/inc/monitorPolicyTable.h $NET_SNMP_MIB_MODULE_PATH

#NetAppMon Files
cp $NURI_UNIX_SRC_PATH/NetAppMon/inc/netApplicationMonitoringTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/NetAppMon/src/netApplicationMonitoringTable.c $NET_SNMP_MIB_MODULE_PATH

#TransMon Files
cp $NURI_UNIX_SRC_PATH/TransMon/inc/transactionTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/TransMon/src/transactionTable.c $NET_SNMP_MIB_MODULE_PATH

#Common Files
cp Nuri/Project/Development/Common/NASCenter_Errors.h $NET_SNMP_MIB_MODULE_PATH
cp Nuri/Project/Development/Common/ctypes.h $NET_SNMP_MIB_MODULE_PATH
cp Nuri/Project/Development/Common/NASCenter_Common.h $NET_SNMP_MIB_MODULE_PATH
cp Nuri/Project/Development/Common/NASCenter_FileTrans.h $NET_SNMP_MIB_MODULE_PATH

#For File Transfer
cp Nuri/Project/Development/Common/NASCenter_Errors.h $NURI_UNIX_INC_G2_PATH
cp Nuri/Project/Development/Common/ctypes.h $NURI_UNIX_INC_G2_PATH
cp Nuri/Project/Development/Common/NASCenter_Common.h  $NURI_UNIX_INC_G2_PATH

#For ICP
cp Nuri/Project/Development/Common/NASCenter_Errors.h $NURI_UNIX_G2_PATH/ICP/inc
cp Nuri/Project/Development/Common/ctypes.h $NURI_UNIX_G2_PATH/ICP/inc
cp Nuri/Project/Development/Common/NASCenter_Common.h  $NURI_UNIX_G2_PATH/ICP/inc

#Copying MIB modules End

#Find the OS Name
OS=`uname -a | cut -d " " -f 1`

#Installing pcap
if ! [ -e "/usr/local/include/pcap/pcap.h" -o -e "/usr/include/pcap/pcap.h" -o -e "/usr/local/include/pcap.h" ]
	then
		echo "Installing pcap"
		gunzip libpcap-0.8.1-316.tar.gz
		tar -xvf libpcap-0.8.1-316.tar
		cd	libpcap-0.8.1
		./configure
		make
		make install
	else
		echo "Pcap already Installed"
fi

if [ $? -ne $ZERO ]
	then
	echo "Pcap Install Failed"
	exit
fi

cd $BUILD_HOME

#File Transfer Lib

cd $NURI_UNIX_SRC_G2_PATH
if [ $OS = $Tru64OS ]
then
	make -f makefile.tru
else
	make -f makefile
fi
if [ $? -ne $ZERO ]
	then
	echo "File Transfer Library install Failed"
	exit
fi
cp ../libs/libNASC_FTP.a /usr/local/lib


#Cleaning and Building Agent
cd $BUILD_HOME
cd $NET_SNMP_PATH
rm -rf config.cache
make distclean

echo "Building for Linux"

cp agent/mibgroup/NuriEnterprise/Linux/src/*.c agent/mibgroup/nuri
cp agent/mibgroup/NuriEnterprise/Linux/inc/*.h agent/mibgroup/nuri
./configure --with-mib-modules="mibII host nuri/cpuTable nuri/System nuri/partitionTable nuri/diskTable nuri/networkTable nuri/Swap nuri/MessageQueue nuri/psRunningTable nuri/fsTable nuri/Semaphore nuri/SharedMemory $MIB_MODULES_G1 $MIB_MODULES_G2" --with-out-mib-modules="ucd_snmp notification agent_mibs utilities host/hr_swinst" --with-libs="-L/usr/local/lib  -lpthread -lpcap -lNASC_FTP" --with-cflags="-I/usr/local/include -I/usr/include/pcap -I."

if [ $? -ne $ZERO ]
	then
	echo "Configure Failed"
	exit
fi
make installlibs
if [ $? -ne $ZERO ]
	then
	echo "Make installlibs Failed"
	exit
fi
make installsbin
if [ $? -ne $ZERO ]
	then
	echo "Make installsbin Failed"
	exit
fi
#make installheaders
if [ $? -ne $ZERO ]
	then
	echo "Make installheaders Failed"
	exit
fi

# added by D.J Park in 2004.03.02[START]
cd $BUILD_HOME
cd $NET_SNMP_PATH
./configure --disable--shared
cd snmplib
make clean;make
cd ../apps
if [ -f snmptrap.c ]
then
        make clean; make
	if [ -f .libs/snmptrap ]
	then
                cp .libs/snmptrap $BUILD_HOME/script/trap.exe
	else
                cp snmptrap $BUILD_HOME/script/trap.exe
	fi

	if [ -f .libs/snmpset ]
	then
                cp .libs/snmpset $BUILD_HOME/script/snmpset
	else
                cp snmpset $BUILD_HOME/script/snmpset
	fi

	if [ -f .libs/snmpget ]
	then
                cp .libs/snmpget $BUILD_HOME/script/snmpget
	else
                cp snmpget $BUILD_HOME/script/snmpget
	fi
fi
# added by D.J Park in 2004.03.02[end]

cd $BUILD_HOME

#Building binary for Master Agent 
echo "Building Master Agent for Linux"
cd  "$MA_PATH"
make -f Makefile.linux
cd $BUILD_HOME

cd $BUILD_HOME

#Building RBM
cp $NURI_UNIX_SRC_PATH/RBM/src/rbm.sh .
sh rbm.sh
if [ $? -ne $ZERO ]
	then
	echo "RBM Build Failed"
	exit
fi

cd $BUILD_HOME
#Building  Group2 ICP

echo " BUILDING ICP..."

cd $NURI_UNIX_G2_PATH/ICP/src
make -f makefile_LINUX clean
make -f makefile_LINUX
cp ICP $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "ICP BUILD FAILED"
	exit
    fi

cd $BUILD_HOME

#Building  Group2 FD

echo " BUILDING FD..."

cd $NURI_UNIX_G2_PATH/FD/src
make -f Makefile_FD_Linux clean
make -f Makefile_FD_Linux
cp FD_* $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "FD BUILD FAILED"
	exit
    fi

cd $BUILD_HOME

echo " BUILDING Backup..."

cd $NURI_UNIX_G2_PATH/BM/src
make -f Makefile_BM_Linux clean
make -f Makefile_BM_Linux
cp Backup* $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "BM BUILD FAILED"
	exit
    fi

cd $BUILD_HOME

echo " BUILDING JOBSCHEDULE..."

cd $NURI_UNIX_G2_PATH/JobSched/src
make -f Makefile_JS_Linux clean
make -f Makefile_JS_Linux
cp JobSchedule* $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "JS BUILD FAILED"
	exit
    fi


cd $BUILD_HOME

echo " BUILDING AM..."

cd $NURI_UNIX_G2_PATH/AM/src
make -f MakefileAM clean
make -f MakefileAM
cp AM_* $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "AM BUILD FAILED"
	exit
fi
cd $BUILD_HOME
echo "Building User Metering "

cd $NURI_UNIX_G2_PATH/UC/src

make -f MakefileLinux clean
make -f MakefileLinux
cp firewall $TARGET_PATH

if [ $? -ne $ZERO ]
	then
	echo "USER METERING BUILD FAILED"
	exit
fi

cd $BUILD_HOME
#Creating Package Directories
echo Creating Package
mkdir -p GkesBuild
mkdir -p GkesBuild/sbin
mkdir -p GkesBuild/conf
mkdir -p GkesBuild/mibs
mkdir -p GkesBuild/lib

#Copying Libraries
echo Copying Libraries
cp /usr/local/lib/libnetsnmp* GkesBuild/lib
cp /usr/local/lib/libpcap* GkesBuild/lib
cp /usr/local/lib/libxml* GkesBuild/lib
cp /usr/local/lib/libNASC_FTP.a GkesBuild/lib


#Copying Binaries
echo Copying Binaries
cp /usr/local/sbin/snmpd GkesBuild/sbin/SmartECOAgent
cp /usr/local/sbin/NASCenterMA GkesBuild/sbin/
#cp /usr/local/sbin/snmptrapd GkesBuild/sbin/
#cp /usr/local/sbin/server_udp GkesBuild/sbin/
cp /usr/local/sbin/rbm GkesBuild/sbin
cp /usr/local/sbin/JobSchedule* GkesBuild/sbin
cp /usr/local/sbin/Backup* GkesBuild/sbin
cp /usr/local/sbin/FD_* GkesBuild/sbin
cp /usr/local/sbin/AM_* GkesBuild/sbin
cp /usr/local/sbin/ICP GkesBuild/sbin
cp /usr/local/sbin/firewall GkesBuild/sbin


#Copying mibs
echo Copying mibs
cp NURI-MIB.mib GkesBuild/mibs/

#Copying conf
echo Copying conf
cp Nuri/DeliveryDocs/Docs/snmpd.conf GkesBuild/conf/
cp snmpd.conf GkesBuild/conf/

#Archiving
echo Archiving Package
cd GkesBuild
tar -cvf SmartECOAgent_$OS.tar *
gzip *.tar
echo Build Complete
