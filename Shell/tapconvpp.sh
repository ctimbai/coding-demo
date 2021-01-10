#!/bin/bash

N=2
NS=(ns1,ns2)
NETNS="/var/run/netns"

if [ ! -d "$NETNS" ]; then
	mkdir $NETNS
else
	rm -rf $NETNS/*
fi

for $ns in $NS
do
	docker run -itd -name $ns busybox
done

for $ns in $NS
do
	pid=`docker inspect --format '{{.State.Pid}}' $ns`
	ln -s /proc/$pid/ns/net /var/run/netns/$ns
done

tapset=`ifconfig|grep "tap"|awk '{print$1}'|cut -d: -f1`

for $tap in $tapset
do
	ip link set $tap netns "ns`echo $tap|tr -cd '[0-9]'`"
done

for ((i=0;i<$N;i++))
do
	docker exec --privileged=true ${NS[i]} ip addr add 192.168.$i.2/24 dev ${tapset[i]}
	docker exec --privileged=true ${NS[i]} ip link set ${tapset[i]} up
	docker exec --privileged=true ${NS[i]} ip route add 192.168.${N-i-1}.0/24 via 192.168.$i.1
done
