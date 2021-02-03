#!/bin/bash

# Find the PID of a process and kill it by the name of process
# You need to replace the "xx" to your serverapp

PID=`ps aux|grep xx | awk 'NR==1{print}' | awk -F " " '{print $2}'`
kill -2 $PID
