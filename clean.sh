#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#

SolarisOS=SunOS
UnixWareOS=UnixWare
AixOS=AIX
HPOS=HP-UX
LinuxOS=Linux
Tru64OS=OSF1
BUILD_HOME=`pwd`
NET_SNMP_PATH=Group1/net-snmp-5.8
NURI_UNIX_G2_PATH=Group2/Unix
OS=`uname -a | cut -d " " -f 1`

cd $NET_SNMP_PATH
make clean
if [ -d apps ]
then
	cd apps
	make clean
fi

cd $BUILD_HOME
cd $BUILD_HOME/Group1/net-snmp-5.8
find . -name "*.o" -exec rm -rf {} \; -print
find . -name "*.lo" -exec rm -rf {} \; -print
cd $BUILD_HOME/Group2
find . -name "*.o" -exec rm -rf {} \; -print
find . -name "*.lo" -exec rm -rf {} \; -print
cd $BUILD_HOME
