#!/bin/sh
#
# @author: D.J Park (dong7603@nuritelecom.com)
#


WORK_DIR=`pwd`
BUILD_DIR=$WORK_DIR/NuriBuild
SCRIPT_DIR=$WORK_DIR/script

OS=`uname -a | cut -d " " -f 1`
VERSION=`uname -r`

UPDATE_FILE=updateAgent.$OS-$VERSION.tar

echo "rm -rf $UPDATE_FILE"
rm -rf $UPDATE_FILE
echo "cp $SCRIPT_DIR/* $BUILD_DIR/sbin/"
cp $SCRIPT_DIR/* $BUILD_DIR/sbin/
echo "cd $BUILD_DIR"
cd $BUILD_DIR
echo "tar -cvf $UPDATE_FILE sbin lib conf"
tar -cvf $UPDATE_FILE sbin lib conf
echo "cp $UPDATE_FILE $WORK_DIR"
cp $UPDATE_FILE $WORK_DIR
echo "cd $WORK_DIR"
cd $WORK_DIR
