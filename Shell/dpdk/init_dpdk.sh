#!/bin/bash

# author: cham
# init env before starting dpdk
# Set the env RTE_SDK and RTE_TARGET before running this script

# Use dpdk-17.05 three
export RTE_SDK=/home/dpdk/dpdk-17.05
export RTE_TARGET=x86_64-native-linuxapp-gcc

if [ $# == 0 ]
then
    echo "Usage: sudo ./init_dpdk dev"
    exit
fi

dev=$1

# bind nic to igb_uio
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

# Insert rte_kni(optional)
lsmod | grep rte_kni >& /dev/null
if [ $? -ne 0 ]; then
    insmod ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko kthread_mode=multiple
fi

if [ ! -d /dev/hugepages ]; then
    mkdir -p /dev/hugepages
fi

# mount 2M hugepages
mount -t hugetlbfs nodev /dev/hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages

bind_nic $dev
