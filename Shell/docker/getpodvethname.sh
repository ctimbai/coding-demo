#!/bin/bash
#filename:getveth.sh
#author:by

container_name=$1
if [ -z $1 ] ; then
echo "Usage: ./getveth.sh container_name"
exit 1
fi

if [ `docker inspect -f "{{.State.Pid}}" ${container_name} &>>/dev/null && echo 0 || echo 1` -eq 1 ];then
echo "no this container:${container_name}"
exit 1
fi

pid=`docker inspect -f "{{.State.Pid}}" ${container_name}`
mkdir -p /var/run/netns
ln -sf /proc/$pid/ns/net "/var/run/netns/${container_name}"

index=`ip netns exec "${container_name}" ip link show eth0 | head -n1 | sed s/:.*//`
let index=index+1
vethname=`ip link show | grep "^${index}:" | sed "s/${index}: \(.*\):.*/\1/"`
echo $vethname
rm -f "/var/run/netns/${container_name}"