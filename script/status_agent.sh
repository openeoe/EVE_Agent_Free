#!/bin/sh

ps -eaf | grep SmartECOAgent | grep -v grep | grep -v agentCheck 
ps -eaf | grep nasgated | grep -v grep 
ps -eaf | grep agentCheck | grep -v grep 
ps -eaf | grep scotty | grep -v grep 
