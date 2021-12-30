#!/bin/sh

HOME=`pwd`
OS=`uname -a | cut -d " " -f 1`
FILE=$HOME/crontab.copy
CRONCONTENT_BODY="30 23,0,1,2 * * *  /usr/local/sbin/updateAgent.sh"
ROOT=root


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
	echo "does not registered $CRONCONTENT"
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
