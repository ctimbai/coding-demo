#!/bin/bash

IMAGE_NAME=ovsdpdk-dev

# config dpdk env
# install uio
modprobe uio
insmod $RTE_SDK/$RTE_TARGET/kmod/igb_uio.ko

# hugepage
# 预分配大页面
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
umount /dev/hugepages/
mkdir -p /dev/hugepages/
chmod 777 /dev/hugepages/
mount -t hugetlbfs nodev /dev/hugepages/

# run ovs-docker
docker run -it --privileged -v /sys/bus/pci/devices:/sys/bus/pci/devices -v /sys/kernel/mm/hugepages:/sys/kernel/mm/hugepages -v /sys/devices/system/node:/sys/devices/system/node -v /dev:/dev $IMAGE_NAME