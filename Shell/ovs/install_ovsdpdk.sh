#!/bin/bash

# OVS版本: ovs2.11
# DPDK版本: dpdk18.11

OVS_DIR=/root/ovs/ovs-2.11
DPDK_DIR=/root/dpdk/dpdk-stable-18.11.8
OVS_INSTALL_DIR=$OVS_DIR/install

RTE_SDK=$DPDK_DIR
RTE_TARGET=x86_64-native-linuxapp-gcc

# 编译DPDK
cd $DPDK_DIR
make config T=x86_64-native-linuxapp-gcc
sed -ri 's,(PMD_PCAP=).*,\1y,' build/.config
make install T=x86_64-native-linuxapp-gcc DESTDIR=install EXTRA_CFLAGS="-g -Ofast"
#调试版加入 EXTRA_CFLAGS="-g -Ofast" 或者 "-g -O0"

# 编译OVS
cd $OVS_DIR
./boot.sh
./configure --with-dpdk=$DPDK_DIR/$RTE_TARGET --prefix=$OVS_INSTALL_DIR --with-debug CFLAGS='-g -Ofast'
# 调试版可加入 --with-debug CFLAGS='-g -Ofast'

make -j8 # 8 lcore同时编译，提高编译速度
make install

# 启动 OVS

# 预分配大页面
#echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
#umount /dev/hugepages/
#mkdir -p /dev/hugepages/
#chmod 777 /dev/hugepages/
#mount -t hugetlbfs nodev /dev/hugepages/

# 清除之前的OVS环境
pkill ovsdb-server 
pkill ovs-vswitchd

rm -rf /usr/local/var/run/openvswitch
rm -rf /usr/local/etc/openvswitch
rm -rf /usr/local/var/log/openvswitch
rm -rf $OVS_INSTALL_DIR/var/run/openvswitch
rm -rf $OVS_INSTALL_DIR/etc/openvswitch
rm -rf $OVS_INSTALL_DIR/var/log/openvswitch

mkdir -p $OVS_INSTALL_DIR/var/run/openvswitch
mkdir -p $OVS_INSTALL_DIR/etc/openvswitch
mkdir -p $OVS_INSTALL_DIR/var/log/openvswitch

# 创建数据库服务器
$OVS_DIR/ovsdb/ovsdb-tool create $OVS_INSTALL_DIR/etc/openvswitch/conf.db $OVS_DIR/vswitchd/vswitch.ovsschema

# 首次运行时要创建初始化数据库
$OVS_DIR/ovsdb/ovsdb-server --remote=punix:$OVS_INSTALL_DIR/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --pidfile --detach

# 启动 ovsdb-server
$OVS_DIR/utilities/ovs-vsctl --no-wait init

# 启动 OVS-DPDK
$OVS_DIR/utilities/ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-init=true
# 为两个socket各分配1G的内存
$OVS_DIR/utilities/ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-socket-mem="512,0"
$OVS_DIR/vswitchd/ovs-vswitchd unix:$OVS_INSTALL_DIR/var/run/openvswitch/db.sock --pidfile --detach

# 将 OVS 的工具加入到PATH中
export PATH=$PATH:$OVS_INSTALL_DIR/share/openvswitch/scripts
export PATH=$PATH:$OVS_INSTALL_DIR/bin
