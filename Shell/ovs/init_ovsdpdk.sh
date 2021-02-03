#!/bin/bash

OVS_DIR=/root/ovs/ovs-2.11
OVS_INSTALL_DIR=$OVS_DIR/install

# 清除之前的OVS环境
pkill ovsdb-server 
pkill ovs-vswitchd

rm -rf /dev/hugepages/*
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

ovs-appctl vlog/set bridge:syslog:dbg
ovs-appctl vlog/set bridge:file:dbg

# add br
$OVS_INSTALL_DIR/bin/ovs-vsctl --may-exist add-br br0 -- set bridge br0 datapath_type=netdev

# add port
$OVS_INSTALL_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user1 -- set Interface vhost-user1 type=dpdkvhostuser
$OVS_INSTALL_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user2 -- set Interface vhost-user2 type=dpdkvhostuser
$OVS_INSTALL_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user3 -- set Interface vhost-user3 type=dpdkvhostuser

# add flows
$OVS_INSTALL_DIR/bin/ovs-ofctl add-flow br0 in_port=1,actions=Output:2
$OVS_INSTALL_DIR/bin/ovs-ofctl add-flow br0 in_port=3,actions=Output:1
