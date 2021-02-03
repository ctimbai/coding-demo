#!/bin/bash

echo $RTE_SDK

print_usage() {
	echo "There need to be one parameter"
	echo "-d :for debug"
	echo "-r :for release"
}

cd $RTE_SDK
make config T=x86_64-native-linuxapp-gcc
#sed -ri 's,(PMD_PCAP=).*,\1y,' build/.config

if [ $# -eq 1 ]
then
    case $1 in
    -d)
    make install T=x86_64-native-linuxapp-gcc DESTDIR=install EXTRA_CFLAGS="-g -Ofast"
    ;;
    -r)
    make install T=x86_64-native-linuxapp-gcc DESTDIR=install
    ;;
    *)
    print_usage
    ;;
    esac
else 
	print_usage
fi