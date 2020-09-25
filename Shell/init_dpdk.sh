#!/bin/bash

# init env before starting dpdk
# https://github.com/alandtsang/mydpdkdns

if [ $# == 0 ]
then
    echo "Usage: sudo ./init_dpdk dev"
    exit
fi

dev=$1

bind_nic() {
    ifconfig $dev down
    ${RTE_SDK}/usertools/dpdk-devbind.py --bind=igb_uio $dev
}

# Insert Module uio
modprobe uio

# Insert igb_uio
lsmod | grep igb_uio >& /dev/null
if [ $? -ne 0 ]; then
    insmod ${RTE_SDK}/${RTE_TARGET}/kmod/igb_uio.ko
fi

# Insert rte_kni
lsmod | grep rte_kni >& /dev/null
if [ $? -ne 0 ]; then
    insmod ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko kthread_mode=multiple
fi

if [ ! -d /dev/hugepages ]; then
    mkdir -p /dev/hugepages
fi

# mount hugepages
mount -t hugetlbfs nodev /dev/hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages

bind_nic $dev
