set -x
RELEASE_VERSION=FINAL_VERSION
rm -rf NuriBuild

# Setting PATH
PATH=/usr/local/bin:/usr/ccs/bin:/usr/contrib/bin:$PATH
export PATH

# Setting PATH
LD_LIBRARY_PATH=/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH

#Some constants
ZERO=0

#End of List of Options

#Some common PATH definitions
NURI_UNIX_SRC_PATH=Group1
NET_SNMP_PATH=Group1/net-snmp-5.8

NURI_UNIX_SRC_G2_PATH=Group2/Unix/common/src
NURI_UNIX_INC_G2_PATH=Group2/Unix/common/inc

NET_SNMP_MIB_MODULE_PATH=Group1/net-snmp-5.8/agent/mibgroup/nuri
MA_PATH="Group1/Master Agent"
BUILD_HOME=`pwd`
#PATH definitions End

#Modules to be compiled
MIB_MODULES="nuri/trapTable nuri/collectDataTable nuri/logMonRowTable  nuri/agentInfo nuri/psMonitorTable"
#End of Modules

#Creating Nuri Directory under  NET-SNMP
mkdir  -p $NET_SNMP_MIB_MODULE_PATH
#Nuri Directory Created


#Copying MIB modules

#AgentInfo Files
cp $NURI_UNIX_SRC_PATH/Common/src/agentInfo.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/Common/inc/agentInfo.h $NET_SNMP_MIB_MODULE_PATH

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

#Copying MIB modules End

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
#Installing xml
if ! [ -e "/usr/local/include/gnome-xml/parser.h" -o -e "/usr/include/gnome-xml/parser.h"  ]
	then
		echo "Installing xml"
		gzip -d libxml-1.8.16.tar.gz
		tar -xvf libxml-1.8.16.tar
    	cd	libxml-1.8.16
		./configure
		make
		make install
	else
		echo "libXML already Installed"
fi

if [ $? -ne $ZERO ]
	then
	echo "XML Install Failed"
	exit
fi

cd $BUILD_HOME

#File Transfer Lib

OS=`uname -a | cut -d " " -f 1`
cd $NURI_UNIX_SRC_G2_PATH
make -f makefile

if [ $? -ne $ZERO ]
	then
	echo "File Transfer Library install Failed"
	exit
fi
cp ../libs/libNASC_FTP.a /usr/local/lib


#Building Agent
cd $BUILD_HOME
echo "Building NASCenterAgent for Linux"

cd $NET_SNMP_PATH
cp agent/mibgroup/NuriEnterprise/Linux/src/*.c agent/mibgroup/nuri
cp agent/mibgroup/NuriEnterprise/Linux/inc/*.h agent/mibgroup/nuri
		./configure --with-mib-modules="mibII host nuri/agentInfo nuri/sysStatisticsTable nuri/subAgentConfig nuri/trapTable nuri/collectDataTable nuri/logMonRowTable nuri/logMonColumnTable nuri/psMonitorTable nuri/cpuTable nuri/System nuri/partitionTable nuri/diskTable nuri/networkTable nuri/Swap nuri/MessageQueue nuri/psRunningTable nuri/fsTable nuri/Semaphore nuri/SharedMemory nuri/monitorPolicyTable nuri/netApplicationMonitoringTable /nuri/transactionTable" --with-out-mib-modules="ucd_snmp notification agent_mibs utilities" --with-libs="-lpthread -lpcap -lNASC_FTP"


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
make installheaders
if [ $? -ne $ZERO ]
	then
	echo "Make installheaders Failed"
	exit
fi

cd $BUILD_HOME

echo "Building Master Agent for Linux"
cd  "$MA_PATH"
make -f Makefile.linux
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


#Creating Package Directories
echo Creating Package
mkdir -p NuriBuild
mkdir -p NuriBuild/sbin
mkdir -p NuriBuild/conf
mkdir -p NuriBuild/mibs
mkdir -p NuriBuild/lib

#Copying Libraries
echo Copying Libraries
cp /usr/local/lib/libnetsnmp* NuriBuild/lib
cp /usr/local/lib/libNASC_FTP.a NuriBuild/lib


#Copying Binaries
echo Copying Binaries
cp /usr/local/sbin/snmpd NuriBuild/sbin/NASCenterAgent
cp /usr/local/sbin/NASCenterMA NuriBuild/sbin/
cp /usr/local/sbin/snmptrapd NuriBuild/sbin/
cp /usr/local/sbin/server_udp NuriBuild/sbin/
cp /usr/local/sbin/rbm NuriBuild/sbin

#Copying mibs
echo Copying mibs
cp Nuri/DeliveryDocs/Docs/NURI-MIB.mib NuriBuild/mibs/

#Copying conf
echo Copying conf
cp Nuri/DeliveryDocs/Docs/*.conf NuriBuild/conf/

#Archiving
echo Archiving Package
cd NuriBuild
tar -cvf NASCenterAgent.tar *
gzip *.tar
echo Build Complete
