#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#


WORK_DIR=`pwd`
SCRIPT_DIR=$WORK_DIR/script
RSYNC_GZ_FILE=rsync-2.6.0.tar.gz
RSYNC_TAR_FILE=rsync-2.6.0.tar
RSYNC_DIR=rsync-2.6.0

cd $WORK_DIR
if [ -f $RSYNC_GZ_FILE ]
then
	gzip -d $RSYNC_GZ_FILE
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
	gzip $RSYNC_TAR_FILE
else
	echo "Does not exist $RSYNC_GZ_FILE"
	exit
fi
