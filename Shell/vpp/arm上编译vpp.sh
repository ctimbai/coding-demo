************************************vpp********************************
yum install -y python-pip kernel-devel-`uname -r`

git clone https://gerrit.fd.io/r/vpp
git clone https://github.com/FDio/vpp.git
cd vpp
git checkout stable/1804
git checkout -b my-19.08 v19.08


vi /etc/yum.repos.d/CentOS-Debuginfo.repo
enable=1
vi build/external/packages/dpdk.mk
RTE_KNI_KMOD   y
RTE_EAL_IGB_UIO y

///////////////////   ./build-root/vagrant/build.sh


make wipe-release
make install-dep
make install-ext-deps   
make build-release

//dpdk 
build-root/build-vpp-native/external/dpdk-19.05/arm64-armv8a-linuxapp-gcc/kmod/igb_uio.ko
make pkg-rpm


cd build-root
rpm -ivh vpp*.rpm
systemctl restart vpp


# vppctl show interface