#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#

WORK_DIR=`pwd`
VERSION=4.2

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

echo "start to generate SmartECOAgent_$VERSION.src.tar.gz"
rm -rf SmartECOAgent_$VERSION.src.tar.gz
$WORK_DIR/clean.sh
tar -cvf SmartECOAgent_$VERSION.src.tar Common Group1 Group2 Nuri Win_Build script NURI-MIB.mib *.sh *.h rsync-2.6.0.tar.gz
gzip SmartECOAgent_$VERSION.src.tar
echo "sucesssfull to generate SmartECOAgent_$VERSION.src.tar.gz"
