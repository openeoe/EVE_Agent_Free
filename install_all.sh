#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#


HOME=`pwd`
INSTALL_DIR=/usr/local
PCAP_FILE=$HOME/Nuri/DeliveryDocs/Docs/libpcap-0.8.1-316.tar.gz
XML_FILE=$HOME/Nuri/DeliveryDocs/Docs/libxml-1.8.16.tar.gz
OS=`uname -a | cut -d " " -f 1`

if [ -f /usr/local/sbin/end_agent.sh ]
then
	/usr/local/sbin/end_agent.sh
fi

cp $PCAP_FILE $HOME
cp $XML_FILE $HOME

./SmartECOAgent_G2_All.sh

cp -f $HOME/NuriBuild/SmartECOAgent_$OS.tar.gz $INSTALL_DIR
rm -rf lib*

cd $INSTALL_DIR
rm -f SmartECOAgent_$OS.tar
gzip -d SmartECOAgent_$OS.tar.gz
tar -xvf SmartECOAgent_$OS.tar
rm -f SmartECOAgent_$OS.tar

cd $HOME
