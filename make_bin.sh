#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#


WORK_DIR=`pwd`
BUILD_DIR=$WORK_DIR/GkesBuild
SCRIPT_DIR=$WORK_DIR/script
INSTALL_DIR=ECOAgentInstall
OS=`uname -a | cut -d " " -f 1`
VERSION=`uname -r`

GZIP=`which gzip`

if [ $? -ne 0 ]
then
    if [ -f /bin/gzip ]
    then
        GZIP=/bin/gzip
    else
        if [ -f /usr/local/bin/gzip ]
        then
            GZIP=/usr/local/bin/gzip 
        else
            echo "can not found gzip"
            exit
        fi
    fi
fi

make_rsync ()
{
	if [ ! -f $SCRIPT_DIR/rsync ]
	then
		RSYNC_GZ_FILE=rsync-2.6.0.tar.gz
		RSYNC_TAR_FILE=rsync-2.6.0.tar
		RSYNC_DIR=rsync-2.6.0
		cd $WORK_DIR
		if [ -f $RSYNC_GZ_FILE ]
		then
			$GZIP -d $RSYNC_GZ_FILE
			tar -xvf $RSYNC_TAR_FILE
			cd $RSYNC_DIR
			./configure
			make
			if [ ! -f rsync ]
			then
				echo "rsync compile failed"
				exit
			fi
			cp rsync $SCRIPT_DIR
			cd $WORK_DIR
			rm -rf $RSYNC_DIR
			$GZIP $RSYNC_TAR_FILE
		else
			echo "Does not $RSYNC_GZ_FILE"
			exit
		fi
	fi
}

if [ $# -ne 1 ]
then
    printf "버젼이 $VERSION(이/가) 맞습니까?(y/n):"
    read command
    if [ ! $command = "y" ]
    then
        echo " 정확한 버젼을 입력해 주세요.."
        exit
    fi
else
    if [ ! $VERSION = $1 ]
    then
        printf "버젼이 $1(이/가) 맞습니까?(y/n):"
        read command
        if [ $command = "y" ]
        then
            VERSION=$1
        else
            echo " 정확한 버젼을 입력해 주세요.."
            exit
        fi
    fi
fi

echo "start to generate SmartECOAgent5.8_$OS.$VERSION.bin.tar.gz"
rm -rf SmartECOAgent5.8_$OS.$VERSION.bin.tar.gz
rm -rf $BUILD_DIR/*.tar.gz
gcc -o conv_ps conv_ps.c
gcc -o conv_log conv_log.c
cp $SCRIPT_DIR/* $BUILD_DIR/sbin/
cd $BUILD_DIR
mkdir $INSTALL_DIR
tar -cvf SmartECOAgent_$OS.tar sbin lib conf mibs
gzip SmartECOAgent_$OS.tar
cp SmartECOAgent_$OS.tar.gz $INSTALL_DIR
cp $GZIP $INSTALL_DIR
cp $WORK_DIR/agent_install.sh $INSTALL_DIR
cp $WORK_DIR/conv_ps $INSTALL_DIR
cp $WORK_DIR/conv_log $INSTALL_DIR
cp $WORK_DIR/mibList.tar $INSTALL_DIR
tar -cvf SmartECOAgent5.8_$OS.$VERSION.bin.tar $INSTALL_DIR
gzip SmartECOAgent5.8_$OS.$VERSION.bin.tar
mv SmartECOAgent5.8_$OS.$VERSION.bin.tar.gz $WORK_DIR
rm -rf $INSTALL_DIR
cd $WORK_DIR
echo "sucessfull to generate SmartECOAgent5.8_$OS.$VERSION.bin.tar.gz"
