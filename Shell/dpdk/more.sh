dbg:
cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"

l3fwd:
cd /home/dengliang/ovs-dbg/
./dpdk-stable-17.05.1/examples/l3fwd-power/build/l3fwd-power -l 4 -n 3 --socket-mem 256 0 --proc-type primary --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1,queue_size=1024 --file-prefix=container1 -- -p 1 -P --config="(0,0,4)" --parse-ptype --no-numa

/sf/mtcp/bin/epwget 10.0.0.1/index.html 100000000 -f /sf/mtcp/etc/epwget.conf -N 1 -c 1000 -i 800

/sf/mtcp/bin/epserver -p /sf/code/www -f /sf/mtcp/etc/epserver.conf -N 8

/sf/mtcp/bin/epwget 10.0.0.1/index.html?jobid=1%27or%271%27=%271 10000000000 -f /sf/mtcp/etc/epwget.conf -N 6 -c 10000 -i 800

docker run -d --name=mtcpperf_epwget --privileged=true --cap-add=all --net=none -v /mnt/huge:/mnt/huge -v /dev:/dev -v /home/baichangmin/mtcpperf/af:/sf -v /home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1:/var/run/usvhost1 -v /home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2:/var/run/usvhost2 ngaf.registry:5000/mtcpperf

cd /home/dengliang/ovs-dbg/
./dpdk-stable-17.05.1/examples/l3fwd-power/build/l3fwd-power -l 4 -n 3 --socket-mem 256 0 --proc-type primary --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2,queue_size=1024 --file-prefix=container2 -- -p 1 -P --config="(0,0,4)" --parse-ptype --no-numa

cd /home/dengliang/ovs-dbg/
./dpdk-stable-17.05.1/examples/l3fwd-power/build/l3fwd-power -l 4 -n 3 --socket-mem 256 0 --proc-type primary --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user3,queue_size=1024 --file-prefix=container3 -- -p 1 -P --config="(0,0,4)" --parse-ptype --no-numa

docker run -d --name=mtcpperf_epwget --privileged=true --cap-add=all --net=none -v /mnt/huge:/run/mtcp/hugepages -v /home/baichangmin/mtcpperf/af:/sf -v /home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1:/var/run/usvhost1 -v /home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2:/var/run/usvhost2 ngaf.registry:5000/mtcpperf

l2fwd-pktgen:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 -b 0000:03:00.0 --socket-mem 128,0 --file-prefix=container1 -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --socket-mem 128,0 --file-prefix=container1 -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 4-5  -n 4 --socket-mem 128,0 --file-prefix=container2 -- -P -m "5.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 12-13  -n 4 --socket-mem 128,0 --file-prefix=container3 -- -P -m "13.0"

l2fwd-pktgen-two-ports:
./dpdk-stable-16.11.2/examples/l2fwd/build/l2fwd -l 3-5 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1 --vdev=virtio_user1,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2  --file-prefix=container -- -p 3 -T 2

l2fwd:
cd /home/dengliang/ovs-dbg/
./dpdk-stable-16.11.2/examples/l2fwd/build/l2fwd -l 2 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1,queue_size=1024 --file-prefix=container1 -- -p 1 -T 2

cd /home/dengliang/ovs-dbg/
./dpdk-stable-16.11.2/examples/l2fwd/build/l2fwd -l 2 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2,queue_size=1024 --file-prefix=container2 -- -p 1 -T 2

cd /home/dengliang/ovs-dbg/
./dpdk-stable-16.11.2/examples/l2fwd/build/l2fwd -l 2 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user3,queue_size=1024 --file-prefix=container3 -- -p 1 -T 2

l2fwd-17.05:
./dpdk-stable-17.05.2/examples/l2fwd/build/l2fwd -l 3-4 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1 --file-prefix=container -- -p 1 -T 2

set int l2 bridge VirtioUser0/0/0/0 100
set int l2 bridge VirtioUser0/0/0/1 100
set int state VirtioUser0/0/0/0 up
set int state VirtioUser0/0/0/1 up

set int feature VirtioUser0/0/0/0 dispatcher arc device-input
set int feature VirtioUser0/0/0/1 dispatcher arc device-input
set int feature VirtioUser0/0/0/0 security arc interface-output
set int feature VirtioUser0/0/0/1 security arc interface-output


ovs抓包：
./openvswitch-2.7.2/utilities/ovs-vsctl add-port br0 dummy0

./openvswitch-2.7.2/utilities/ovs-vsctl -- --id=@m create mirror name=mirror0 \
-- add bridge br0 mirrors @m

./openvswitch-2.7.2/utilities/ovs-vsctl list port dummy0
d3427810-8e68-40af-99c0-8cb935af9882
./openvswitch-2.7.2/utilities/ovs-vsctl set mirror mirror0 \
output_port=d3427810-8e68-40af-99c0-8cb935af9882
 
./openvswitch-2.7.2/utilities/ovs-vsctl set mirror mirror0 select_all=1
测试
tcpdump -i dummy0


dispatcher macswap VirtioUser0/0/0/0
dispatcher macswap VirtioUser0/0/0/1
security detect VirtioUser0/0/0/0
security detect VirtioUser0/0/0/1

set int feature VirtioUser0/0/0/0 dispatcher arc ethernet-input
set int feature VirtioUser0/0/0/1 dispatcher arc ethernet-input
set int feature VirtioUser0/0/0/0 security arc interface-output
set int feature VirtioUser0/0/0/1 security arc interface-output

script test/set_seq64.lua
script test/set_seq128.lua
script test/set_seq256.lua
script test/set_seq512.lua
script test/set_seq1024.lua
script test/set_seq1518.lua
script test/set_seq64k.lua

taskset -p 0x80 3705
taskset -p 0x100 3706
taskset -p 0x200 3707

taskset -p 0x80 3747
taskset -p 0x100 3748
taskset -p 0x200 3749

docker run -d --name=ngaf_dataplane --privileged=true --cap-add=all --net=none -v /mnt/huge:/run/vpp/hugepages -v $PWD:/sf/code -v /boot/sf/vpp:/sf/vpp -v /home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1:/var/run/usvhost2 -v /home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2:/var/run/usvhost3 ngaf.registry:5000/dataplane

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1,queue_size=8192    --file-prefix=container     -- -P -m "3.0"

./examples/l2fwd/l2fwd/x86_64-native-linuxapp-gcc/l2fwd -c f -n 3 -- -q 8 -p f
./examples/l2fwd/l2fwd/x86_64-native-linuxapp-gcc/l2fwd -l 4 -n 3 --socket-mem 256,0 --proc-type primary --file-prefix=container1 -- -q 8 -p f
./examples/l2fwd/l2fwd/x86_64-native-linuxapp-gcc/l2fwd -l 4 -n 3 --socket-mem 256,0 --proc-type primary --no-pci --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2,queue_size=1024 --file-prefix=container1 -- -q 1 -p 1

./examples/l2fwd/l2fwd/x86_64-native-linuxapp-gcc/l2fwd -l 4-5 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2 --file-prefix=container1 --  -p 1 –T 2

./examples/l2fwd/build/l2fwd -l 4-5 -n 3 --socket-mem 128,0 --proc-type auto --no-pci --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2 --file-prefix=container1 --  -p 1 –T 2

-l 4 -n 3 --socket-mem 256 0 --proc-type primary --no-pci --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2,queue_size=1024 --file-prefix=container1 -- -p 1 -P --config="(0,0,4)" --parse-ptype --no-numa

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 4-5 -n 4   --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2    --file-prefix=container2     -- -P -m "5.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 4-5 -n 4   --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2,queue_size=8192    --file-prefix=container2     -- -P -m "5.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 16-17  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user4    --file-prefix=container4     -- -P -m "17.0"

./openvswitch-2.7.2/ovsdb/ovsdb-tool create ./etc/openvswitch/conf.db ./share/openvswitch/vswitch.ovsschema
			
./openvswitch-2.7.2/ovsdb/ovsdb-server --remote=punix:db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --pidfile --detach

./openvswitch-2.7.2/vswitchd/ovs-vswitchd unix:db.sock --pidfile --detach
		
cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --no-pci     --proc-type=secondary --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container1     -- -P -m "3.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 2-3 -n 4 --socket-mem 128,0 --no-pci --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user2    --file-prefix=container1     -- -P -m "3.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -c 0x41  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "6.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -c 0x1040  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "12.0"

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -c 0x1040  -n 4 --socket-mem 0,128 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "6.0"

set args -l 2-3  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"

b eal_memory.c:1285
script test/set_seq.lua



new:
cd /home/baichangmin/ovs-new/dpdk-stable-17.05.2/pktgen-3.3.8;
./app/x86_64-native-linuxapp-gcc/pktgen  -l 2-3  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-new/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"

cd /home/baichangmin/ovs-new/dpdk-stable-17.05.2/pktgen-3.3.8;
./app/x86_64-native-linuxapp-gcc/pktgen -l 4-5 -n 4   --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/baichangmin/ovs-new/var/run/openvswitch/vhost-user2    --file-prefix=container2     -- -P -m "5.0"

perf top -p 1577

cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/lib/librte_eal/linuxapp/eal/
cd /home/baichangmin/ovs-dbg/dpdk-stable-16.11.2/lib/librte_vhost/




前端：
ret = vhost_user_sock(dev->vhostfd, VHOST_USER_SET_MEM_TABLE, NULL);
后端：
prepare_vhost_memory_user
			
			原方案				我的
1518	812800/812416		812832/812800
		812800/810976		812832/812800
64		2062752/2038240		3681632/3667040
		2076416/2040224		3683456/3663360
512		1847808/1675616		3681632/2348288	
		1789600/1676384		3684512/2333984
		
2.6.1:
cd /home/dengliang/ovs-2.6.1/dpdk-stable-16.07.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 2-3 -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/dengliang/ovs-2.6.1/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"

cd /home/dengliang/ovs-2.6.1/dpdk-stable-16.07.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 4-5 -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/dengliang/ovs-2.6.1/var/run/openvswitch/vhost-user2    --file-prefix=container2     -- -P -m "5.0"



cd /sf/data/local/dengliang/ovs/dpdk-16.07/pktgen-dpdk-pktgen-3.3.4/;
./app/app/x86_64-ivshmem-linuxapp-gcc/pktgen -l 2-3 -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user1    --file-prefix=container     -- -P -m "3.0"


cd /sf/data/local/dengliang/ovs/dpdk-16.07/pktgen-dpdk-pktgen-3.3.4/;
./app/app/x86_64-ivshmem-linuxapp-gcc/pktgen -l 4-5 -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/home/dengliang/ovs-dbg/var/run/openvswitch/vhost-user2    --file-prefix=container2     -- -P -m "5.0"






cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen --huge-dir=/mnt/huge  -l 4-5  -n 4 --socket-mem 512,0    --vdev 'eth_vhost0,iface=/tmp/sock0'     --file-prefix=host --no-pci -- -P -m "5.0"


cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen -l 2-3 -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0     --file-prefix=container     -- -P -m "3.0"


virtio-user:
singleport-singlecore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;rm -rf /tmp/sock0;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 -l 2-3 --lcores='2,3' --socket-mem 128,0 --no-pci   --vdev 'eth_vhost0,iface=/tmp/sock0'  --file-prefix=host     -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 -l 2-3 --lcores='4,5' --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0  --file-prefix=container     -- -P -m "5.0"

singleport-singlecore-nonuma:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;rm -rf /tmp/sock0;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci    --vdev 'eth_vhost0,iface=/tmp/sock0'  --file-prefix=host     -- -P -m "13.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0  --file-prefix=container     -- -P -m "15.0"



singleport-singlecore-dequeue:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;rm -rf /tmp/sock0;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci    --vdev 'eth_vhost0,iface=/tmp/sock0,dequeue-zero-copy=1'  --file-prefix=host     -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0  --file-prefix=container     -- -P -m "5.0"



singleport-multicore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;rm -rf /tmp/sock0;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci    --vdev 'eth_vhost0,iface=/tmp/sock0'  --file-prefix=host     -- -P -m "[2:3].0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0  --file-prefix=container     -- -P -m "[4:5].0"

multiport-singlecore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;rm -rf /tmp/sock0;rm -rf /tmp/sock1;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,0 --no-pci    --vdev 'eth_vhost0,iface=/tmp/sock0'  --vdev 'eth_vhost1,iface=/tmp/sock1'  --file-prefix=host     -- -P -m "2.0,3.1"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,0 --no-pci     --vdev=virtio_user0,path=/tmp/sock0 --vdev=virtio_user1,path=/tmp/sock1  --file-prefix=container     -- -P -m "4.0,5.1"




vecring:
singleport-singlecore-numa:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci   --huge-dir=/mnt/huge  --vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0    --file-prefix=container     -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci  --huge-dir=/mnt/huge    --vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f    --file-prefix=container2     -- -P -m "5.0"

singleport-singlecore-nonuma:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci   --huge-dir=/mnt/huge  --vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0    --file-prefix=container     -- -P -m "13.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci  --huge-dir=/mnt/huge    --vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f    --file-prefix=container2     -- -P -m "15.0"


singleport-multicore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci   --huge-dir=/mnt/huge  --vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0    --file-prefix=container     -- -P -m "[2:3].0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci  --huge-dir=/mnt/huge    --vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f    --file-prefix=container2     -- -P -m "[4:5].0"


multiport(same_numa)-singlecore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,256 --no-pci   --huge-dir=/mnt/huge  \
--vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0  \
--vdev=eth_vecring1,domain=testcontainer,link=tap458,master=true,mac=00:ec:f4:bb:d9:7f,socket=0  \
    --file-prefix=container     -- -P -m "2.0,3.1"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,256 --no-pci   --huge-dir=/mnt/huge  \
--vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f,socket=0  \
--vdev=eth_vecring1,domain=testcontainer,link=tap458,mac=00:ec:f4:bb:d9:7f,socket=0  \
    --file-prefix=container2     -- -P -m "4.0,5.1"

multiport(diff_numa)-singlecore:
cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,256 --no-pci   --huge-dir=/mnt/huge  \
--vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0  \
--vdev=eth_vecring1,domain=testcontainer,link=tap458,master=true,mac=00:ec:f4:bb:d9:7f,socket=1  \
    --file-prefix=container     -- -P -m "2.0,13.1"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 256,256 --no-pci   --huge-dir=/mnt/huge  \
--vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f,socket=0  \
--vdev=eth_vecring1,domain=testcontainer,link=tap458,mac=00:ec:f4:bb:d9:7f,socket=1  \
    --file-prefix=container2     -- -P -m "4.0,15.1"





cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci   --huge-dir=/mnt/huge  --vdev=eth_vecring0,domain=testcontainer,link=tap456,master=true,mac=00:ec:f4:bb:d9:7f,socket=0,queue=131072    --file-prefix=container     -- -P -m "3.0"

cd /home/dengliang/ovs-dbg/dpdk-stable-16.11.2/pktgen-3.3.8;
./app/app/x86_64-native-linuxapp-gcc/pktgen  -n 4 --socket-mem 128,0 --no-pci  --huge-dir=/mnt/huge    --vdev=eth_vecring0,domain=testcontainer,link=tap456,mac=00:ec:f4:bb:d9:7f,socket=0,queue=131072    --file-prefix=container2     -- -P -m "5.0"
