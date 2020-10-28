## 准备工作

首先通过 `modinfo tun` 查看系统内核是否支持 tap/tun 设备驱动。

```
[root@by ~]# modinfo tun
filename:       /lib/modules/3.10.0-862.14.4.el7.x86_64/kernel/drivers/net/tun.ko.xz
alias:          devname:net/tun
alias:          char-major-10-200
license:        GPL
author:         (C) 1999-2004 Max Krasnyansky <maxk@qualcomm.com>
description:    Universal TUN/TAP device driver
retpoline:      Y
rhelversion:    7.5
srcversion:     50878D5D5A0138445B25AA8
depends:
intree:         Y
vermagic:       3.10.0-862.14.4.el7.x86_64 SMP mod_unload modversions
signer:         CentOS Linux kernel signing key
sig_key:        E4:A1:B6:8F:46:8A:CA:5C:22:84:50:53:18:FD:9D:AD:72:4B:13:03
sig_hashalgo:   sha256
```

在 linux 2.4 及之后的内核版本中，tun/tap 驱动是默认编译进内核中的。

如果你的系统不支持，请先选择手动编译内核或者升级内核。编译时开启选项：

```
Device Drivers => Network device support => Universal TUN/TAP device driver support
```

tap/tun 也支持编译成模块，如果编译成模块，需要手动加载它：

```
[root@localhost ~]# modprobe tun
[root@localhost ~]# lsmod | grep tun
tun                    31665  0
```

关于以上的详细步骤，网上有很多教程，这里就不再赘述了。

https://blog.csdn.net/lishuhuakai/article/details/70305543

上面只是加载了 tap/tun 模块，要完成 tap/tun 的编码，还需要有设备文件，运行命令：

```
mknod /dev/net/tun c 10 200 # c表示为字符设备，10和200分别是主设备号和次设备号
```

这样在 `/dev/net` 就能看到一个名为 tun 的文件。


## 编程示例
### 启动设备

使用 tap/tun 设备，需要先进行一些初始化工作，如下代码所示：

```C
int tun_alloc(char *dev, int flags)
{
    assert(dev != NULL);

    struct ifreq ifr;
    int fd, err;

    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;
    
    if (*dev != '\0') {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }
    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }

    // 一旦设备开启成功，系统会给设备分配一个名称，对于tun设备，一般为tunX，X为从0开始的编号；
    // 对于tap设备，一般为tapX
    strcpy(dev, ifr.ifr_name);

    return fd;
}
```

首先打开字符设备文件 `/dev/net/tun`，然后用 `ioctl` 注册设备的工作模式，是 tap 还是 tun。这个模式由结构体 `struct ifreq` 的属性 `ifr_flags` 来定义，它有以下含义：

- IFF\_TUN: 表示创建一个 tun 设备。
- IFF\_TAP: 表示创建一个 tap 设备。
- IFF\_NO\_PI: 表示不包含包头信息，默认的，每个数据包传到用户空间时，都会包含一个附加的包头来保存包信息，这个表示不加包头。
- IFF\_ONE\_QUEUE：表示采用单一队列模式。

还是有一个属性是 `ifr_name`，表示设备的名称，如果不指定名称的话，系统会给设备自动分配一个，如 `tapX`、`tunX`，X 从 0 开始编号。

`ioctl` 完了之后，文件描述符 fd 就和设备建立起了关联。


### 写一个 ICMP 的调用函数

为了测试上面的程序，我们写一个简单的 ICMP echo 程序。我们会使用 tun 设备，然后给 `tunX` 接口发送一个 ping 包，程序简单处理之后响应这个包，完成 ICMP 的 request 和 reply 的功能。

如下代码所示：

```C
int main()
{
    int tun_fd, nread;
    char buffer[4096];
    char tun_name[IFNAMSIZ];

    tun_name[0] = '\0';

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *        IFF_NO_PI - Do not provide packet information
     */
    tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);

    if (tun_fd < 0) {
        perror("Allocating interface");
        exit(1);
    }

    printf("Open tun/tap device: %s for reading...\n", tun_name);
    
    while (1) {
        unsigned char ip[4];
        // 收包
        nread = read(tun_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("Reading from interface");
            close(tun_fd);
            exit(1);
        }
        
        printf("Read %d bytes from tun/tap device\n", nread);
        
        // 简单对收到的包调换一下顺序
        memcpy(ip, &buffer[12], 4);
        memcpy(&buffer[12], &buffer[16], 4);
        memcpy(&buffer[16], ip, 4);

        buffer[20] = 0;
        *((unsigned short *)&buffer[22]) += 8;
        
        // 发包
        nread = write(tun_fd, buffer, nread);

        printf("Write %d bytes to tun/tap device, that's %s\n", nread, buffer);
    }
    return 0;
}
```

下面测试一下。


### 给 tap/tun 设备配置 IP 地址

编译：

```
[root@localhost coding]# gcc -o taptun taptun.c
[root@localhost coding]# ./taptun
Open tun/tap device: tun0 for reading...
```

开另一个终端，查看生成了 `tun0` 接口：

```
[root@localhost coding]# ip a
6: tun0: <POINTOPOINT,MULTICAST,NOARP> mtu 1500 qdisc noop state DOWN qlen 500
    link/none
```

给 `tun0` 接口配置 IP 并启用，比如 `10.1.1.2/24`。

```
[root@localhost ~]# ip a a 10.1.1.2/24 dev tun0
[root@localhost ~]# ip l s tun0 up
```

再开一个终端，用 `tcpdump` 抓 `tun0` 的包。

```
[root@localhost ~]# tcpdump -nnt -i tun0
```

然后在第二个终端 `ping` 一下 `10.1.1.0/24` 网段的 IP，比如 `10.1.1.3`，看到：

```
[root@localhost ~]# ping -c 4 10.1.1.3
PING 10.1.1.3 (10.1.1.3) 56(84) bytes of data.
64 bytes from 10.1.1.3: icmp_seq=1 ttl=64 time=0.133 ms
64 bytes from 10.1.1.3: icmp_seq=2 ttl=64 time=0.188 ms
64 bytes from 10.1.1.3: icmp_seq=3 ttl=64 time=0.092 ms
64 bytes from 10.1.1.3: icmp_seq=4 ttl=64 time=0.110 ms

--- 10.1.1.3 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3290ms
rtt min/avg/max/mdev = 0.092/0.130/0.188/0.038 ms
```

由于 `tun0` 接口建好之后，会生成一条到本网段 `10.1.1.0/24` 的默认路由，根据默认路由，数据包会走 `tun0` 口，所以能 ping 通，可以用 `route -n` 查看。

再看抓包终端，成功显示 ICMP 的 request 包和 reply 包。

```
[root@localhost ~]# tcpdump -nnt -i tun0
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on tun0, link-type RAW (Raw IP), capture size 262144 bytes
IP 10.1.1.2 > 10.1.1.3: ICMP echo request, id 3250, seq 1, length 64
IP 10.1.1.3 > 10.1.1.2: ICMP echo reply, id 3250, seq 1, length 64
IP 10.1.1.2 > 10.1.1.3: ICMP echo request, id 3250, seq 2, length 64
IP 10.1.1.3 > 10.1.1.2: ICMP echo reply, id 3250, seq 2, length 64
```

再看程序 `taptun.c` 的输出：

```
[root@localhost coding]# ./taptun
Open tun/tap device: tun0 for reading...
Read 48 bytes from tun/tap device
Write 48 bytes to tun/tap device
Read 48 bytes from tun/tap device
Write 48 bytes to tun/tap device
```

ok，以上便验证了程序的正确性。

## 总结

通过这个小例子，让我们知道了基于 tap/tun 编程的流程，对 tap/tun 又加深了一层理解。

使用 tap/tun 设备需要包含头文件 `#include <linux/if_tun.h>`，以下是完整代码。

```C
/******************************************************************************
 *	File Name: taptun.c
 *	Author: 公众号: CloudDeveloper
 *	Created Time: 2019年02月23日 星期六 21时28分24秒
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <linux/if_tun.h>

int tun_alloc(char *dev, int flags)
{
    assert(dev != NULL);

    struct ifreq ifr;
    int fd, err;

    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;
    
    if (*dev != '\0') {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }
    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }

    // 一旦设备开启成功，系统会给设备分配一个名称，对于tun设备，一般为tunX，X为从0开始的编号；
    // 对于tap设备，一般为tapX
    strcpy(dev, ifr.ifr_name);

    return fd;
}

int main()
{
    int tun_fd, nread;
    char buffer[4096];
    char tun_name[IFNAMSIZ];

    tun_name[0] = '\0';

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *        IFF_NO_PI - Do not provide packet information
     */
    tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);

    if (tun_fd < 0) {
        perror("Allocating interface");
        exit(1);
    }

    printf("Open tun/tap device: %s for reading...\n", tun_name);
    
    while (1) {
        unsigned char ip[4];
        // 收包
        nread = read(tun_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("Reading from interface");
            close(tun_fd);
            exit(1);
        }
        
        printf("Read %d bytes from tun/tap device\n", nread);
        
        // 简单对收到的包调换一下顺序
        memcpy(ip, &buffer[12], 4);
        memcpy(&buffer[12], &buffer[16], 4);
        memcpy(&buffer[16], ip, 4);

        buffer[20] = 0;
        *((unsigned short *)&buffer[22]) += 8;
        
        // 发包
        nread = write(tun_fd, buffer, nread);

        printf("Write %d bytes to tun/tap device, that's %s\n", nread, buffer);
    }
    return 0;
}
```
