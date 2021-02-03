#!/bin/bash

OVS_DIR=/root/ovs/ovs-2.11/install

# start ovs
#$OVS_DIR/share/openvswitch/scripts/ovs-ctl --system-id=random start

# add br
$OVS_DIR/bin/ovs-vsctl --may-exist add-br br0 -- set bridge br0 datapath_type=netdev

# add port
$OVS_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user1 -- set Interface vhost-user1 type=dpdkvhostuser
$OVS_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user2 -- set Interface vhost-user2 type=dpdkvhostuser
$OVS_DIR/bin/ovs-vsctl --may-exist add-port br0 vhost-user3 -- set Interface vhost-user3 type=dpdkvhostuser

# add flows
$OVS_DIR/bin/ovs-ofctl add-flow br0 in_port=1,actions=Output:2
$OVS_DIR/bin/ovs-ofctl add-flow br0 in_port=3,actions=Output:1
