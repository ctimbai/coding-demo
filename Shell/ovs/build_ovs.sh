#!/bin/bash

# init env
BASEDIR=/root
OVSVERSION=2.11
OVS=ovs
DPDKVERSION=18.11
DPDK=dpdk
DPDKROOT=$BASEDIR/$DPDK-$DPDKVERSION
OVSROOT=$BASEDIR/$OVS-$OVSVERSION
OVS_INSTALL_DIR=/usr/local

CONFIG=x86_64-native-linuxapp-gcc

# set env
export RTE_SDK=$DPDKROOT
export RTE_TARGET=$CONFIG

# 编译OVS
cd $OVSROOT
./boot.sh
./configure --with-dpdk=$DPDKROOT/$CONFIG --prefix=$OVS_INSTALL_DIR
# 调试版可加入 --with-debug CFLAGS='-g -Ofast'

make -j8 # 8 lcore同时编译，提高编译速度
make install

# 启动 OVS
# 清除之前的OVS环境
Kill all ovsdb-server ovs-vswitchd
rm -rf $OVS_INSTALL_DIR/var/run/openvswitch
rm -rf $OVS_INSTALL_DIR/etc/openvswitch

mkdir -p $OVS_INSTALL_DIR/var/run/openvswitch
mkdir -p $OVS_INSTALL_DIR/etc/openvswitch

# 创建数据库服务器
$OVSROOT/ovsdb/ovsdb-tool create $OVS_INSTALL_DIR/etc/openvswitch/conf.db $OVSROOT/vswitchd/vswitch.ovsschema

# 首次运行时要创建初始化数据库
$OVSROOT/ovsdb/ovsdb-server --remote=punix:$OVS_INSTALL_DIR/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --pidfile --detach

# 启动 ovsdb-server
$OVSROOT/utilities/ovs-vsctl --no-wait init

# 启动 OVS-DPDK
$OVSROOT/utilities/ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-init=true
# 为两个socket各分配1G的内存
$OVSROOT/utilities/ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-socket-mem="512,0"
$OVSROOT/vswitchd/ovs-vswitchd unix:$OVS_INSTALL_DIR/var/run/openvswitch/db.sock --pidfile --detach

# 将 OVS 的工具加入到PATH中
#export PATH=$PATH:$OVS_INSTALL_DIR/share/openvswitch/scripts
#export PATH=$PATH:$OVS_INSTALL_DIR/ovsinstall/bin
