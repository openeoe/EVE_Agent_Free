#!/bin/sh

#/bin/ps -ef | grep nasgated | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x 
/bin/ps -ef | grep SmartECOAgent | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x 
/bin/ps -ef | grep nasgated | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x 
/bin/ps -ef | grep scotty | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x 
/bin/ps -ef | grep agentCheck.sh | grep -v grep | awk '{print $2}' | sed -e "s/^/\/bin\/kill -9 /" | /bin/sh -x 
