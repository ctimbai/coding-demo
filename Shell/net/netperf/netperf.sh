#!/bin/bash

# 跑10条流，测试时间为30s
i=0
while [ $i -lt 10 ]
do
i=$((i+1))
netperf -H 192.168.1.2 -l 30 -- -m size > ${i}.txt &
# netperf -t UDP_STREAM -H 192.168.1.2 -l 30 -- -m size > ${i}.txt &
done
