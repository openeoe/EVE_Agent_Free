#!/bin/sh

OS=`uname -a | cut -d " " -f 1` 
if [ $OS = "Linux" ]
then
    progs=`ps -e | grep agentCheck | grep -v grep | awk '{print $1}'`
else
    progs=`ps -ef | grep agentCheck | grep -v grep | awk '{print $2}'`
fi

for prog in $progs
do
	echo " agentCheck is already running [pid $prog]"
	echo " kill the agentCheck "
	kill -9 $prog
done

progs=`ps -ef | grep SmartECOAgent | grep -v grep | awk '{print $2}'`
if [ "x$progs" = "x" ]
then
	echo " SmartECOAgent does not running"
	exit
fi

nohup /usr/local/sbin/agentCheck.sh -i 60 -f /usr/local/sbin/eco_agent.log -c 0 -u root,nobody -p 80 -m 100000 -s 2 -n SmartECOAgent > /dev/null &
