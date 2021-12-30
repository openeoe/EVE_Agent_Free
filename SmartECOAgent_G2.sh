#!/bin/sh

set -x
# Setting PATH
PATH=/usr/bin:/usr/local/bin:/usr/ccs/bin:/usr/contrib/bin:$PATH
export PATH

#Constants
ZERO=0

gcc -v
if [ $? -ne $ZERO ]
	then
	echo "gcc not found. Please set gcc path in PATH variable"
	exit
fi

########################################################################
# Some common PATH definitions
########################################################################
NURI_UNIX_SRC_PATH=Group1
NET_SNMP_PATH=Group1/net-snmp-5.8
NET_SNMP_MIB_MODULE_PATH=Group1/net-snmp-5.8/agent/mibgroup/nuri
BUILD_HOME=`pwd`

########################################################################
# Clean temporary directories
########################################################################
rm -rf GkesBuild
rm -rf /usr/local/conf/snmpd.conf
rm -rf /usr/local/lib/libnetsnmp*

########################################################################
# Setting PATH
########################################################################
LD_LIBRARY_PATH=/usr/lib:/usr/ccs/lib:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

########################################################################
# Modules to be compiled
########################################################################
MIB_MODULES_G1="nuri/agentInfo \
nuri/sysStatisticsTable \
nuri/logMonRowTable  \
nuri/logMonColumnTable \
nuri/logCollectorTable \
nuri/psMonitorTable \
nuri/fileMonitorTable \
nuri/portMonitorTable \
nuri/trapTable"

########################################################################
# Create Gkes Directory under NET-SNMP
########################################################################
mkdir -p $NET_SNMP_MIB_MODULE_PATH

########################################################################
# Copying MIB modules
########################################################################
cp $NURI_UNIX_SRC_PATH/Common/src/agentInfo.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/Common/inc/agentInfo.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/ProcMon/src/psMonitorTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/ProcMon/inc/psMonitorTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/FileMon/src/fileMonitorTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/FileMon/inc/fileMonitorTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/PortMon/src/portMonitorTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/PortMon/inc/portMonitorTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogCol/src/logCollectorTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogCol/inc/logCollectorTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/src/logMonRowTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/inc/logMonRowTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/src/logMonColumnTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/LogMon/inc/logMonColumnTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/sysStats/inc/sysStatisticsTable.h $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/sysStats/src/sysStatisticsTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RTCM/src/trapTable.c $NET_SNMP_MIB_MODULE_PATH
cp $NURI_UNIX_SRC_PATH/RTCM/inc/trapTable.h $NET_SNMP_MIB_MODULE_PATH

cp Nuri/Project/Development/Common/NASCenter_Errors.h $NET_SNMP_MIB_MODULE_PATH
cp Nuri/Project/Development/Common/ctypes.h $NET_SNMP_MIB_MODULE_PATH
cp Nuri/Project/Development/Common/NASCenter_Common.h $NET_SNMP_MIB_MODULE_PATH


########################################################################
# Cleaning and Building Agent
########################################################################
cd $BUILD_HOME
cd $NET_SNMP_PATH
rm -rf config.cache
make distclean

echo "Building for Linux"
./configure --with-mib-modules="mibII host nuri/cpuTable nuri/System nuri/partitionTable nuri/diskTable nuri/networkTable $MIB_MODULES_G1" --with-out-mib-modules="ucd_snmp agent_mibs utilities host/hr_swinst" --without-openssl --with-enterprise-oid=46971 --with-enterprise-sysoid=.1.3.6.1.4.1.46971.3.2  --with-libs="-L/usr/local/lib  -lpthread" --with-cflags="-I/usr/local/include -I." 


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

########################################################################
# Build utility
########################################################################
cd $BUILD_HOME

cd $NET_SNMP_PATH
./configure --without-openssl --disable--shared
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

########################################################################
# Creating Package Directories
########################################################################
cd $BUILD_HOME
echo Creating Package
mkdir -p GkesBuild
mkdir -p GkesBuild/sbin
mkdir -p GkesBuild/conf
mkdir -p GkesBuild/mibs
mkdir -p GkesBuild/lib

########################################################################
#Copying 
########################################################################
echo Copying
cp /usr/local/lib/libnetsnmp* GkesBuild/lib
mv /usr/local/sbin/snmpd GkesBuild/sbin/SmartECOAgent
cp NURI-MIB.mib GkesBuild/mibs/
cp Nuri/DeliveryDocs/Docs/snmpd.conf GkesBuild/conf/

########################################################################
#Archiving
########################################################################
OS=`uname -a | cut -d " " -f 1`;

echo Archiving Package
cd GkesBuild
tar -cvf SmartECOAgent_$OS.tar *
gzip *.tar
echo Build Complete
