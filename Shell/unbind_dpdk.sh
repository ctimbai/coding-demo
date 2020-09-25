#!/bin/bash

# unbind dpdk nic to other drivers, e.g. e1000

# like "0000:03:00.0 '82599ES 10-Gigabit SFI/SFP+ Network Connection 10fb' if=ens1f0 drv=ixgbe unused=igb_uio,vfio-pci"
get_first_row() {
    ${RTE_SDK}/usertools/dpdk-devbind.py --status | grep "0000:" | awk 'NR==1{print}'
}

# like ens1f0
get_nic_name() {
    ${RTE_SDK}/usertools/dpdk-devbind.py --status | grep $1 | awk -F"=" '{print $2}' | awk -F" " '{print $1}'
}

FIRST_ROW=`get_first_row`
NIC_STRING=`echo $FIRST_ROW | awk -F" " '{print $1}'`

# rebind e1000
${RTE_SDK}/usertools/dpdk-devbind.py --bind=e1000 $NIC_STRING | awk -F"=" '{print $2}'

NIC_NAME=`get_nic_name $NIC_STRING`

# up nic
ifconfig $NIC_NAME up
