# libcpp
也是一个造轮子的地方。

本项目是基于 Muduo 的扩展。基本的 Reactor 组件及 TCP C/S 是Muduo 改写的 C++11 版本。

## 目录结构

```
.
├── app 示例应用
├── src
│   ├── datetime  日期时间 / 定时器
│   ├── http      http服务器/客户端
│   ├── io        io缓冲区等
│   ├── logging   日志
│   ├── kcp       KCP服务器/客户端
│   ├── net       TCP/UDP
│   ├── reactor   reactor组件
│   ├── thread    线程池/锁
│   └── utils     标记类
└── testsuite     测试
```

# 第三方依赖

1. [http-parser]( https://github.com/nodejs/http-parser )
2. [ikcp]()

# 功能

1. 双缓冲异步日志库
2. 层级时间轮
3. 多种协议服务器/客户端：TCP / UDP / KCP / HTTP
4. 线程池
5. 信号量 / 读写锁 / 自旋锁

# Feature

1. one loop per thread 模型
2. reactor 模式

# 性能测试

## HTTP 服务器
测试环境：1核 2G 1Mbps
测试命令：`webbench -c 1000 -t 60  http://106.52.193.215:9981/index.html`
```shell
# 单线程
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://106.52.193.215:9981/index.html
1000 clients, running 60 sec.

Speed=4251 pages/min, 8068 bytes/sec.
Requests: 4063 susceed, 188 failed.

# 双线程
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://106.52.193.215:9981/index.html
1000 clients, running 60 sec.

Speed=5677 pages/min, 10120 bytes/sec.
Requests: 5583 susceed, 94 failed.
```

## 日志
测试环境：WSL Ubuntu 18.04 LTS，i5- 8265U，RAM 8G，单线程写入100W条日志

`ostringstream` 非 thread_local

|                       | /dev/null  | /tmp/log   | ./test_log | ./test_log_mt |
| --------------------- | ---------- | ---------- | ---------- | ------------- |
| 写入字节数（bytes）   | 139888890  | 139888890  | 139888890  | 139888890     |
| 每秒写入条数（msg/s） | 1036435.16 | 1005866.21 | 831328.45  | 848860.40     |
| 吞吐量（MiB/s）       | 138.27     | 134.19     | 110.91     | 113.25        |

`ostringstream` 为 thread_local

|                       | /dev/null  | /tmp/log   | ./test_log | ./test_log_mt |
| --------------------- | ---------- | ---------- | ---------- | ------------- |
| 写入字节数（bytes）   | 139888890  | 139888890  | 139888890  | 139888890     |
| 每秒写入条数（msg/s） | 1475539.97 | 1346251.97 | 1129420.27 | 1092732.56    |
| 吞吐量（MiB/s）       | 196.85     | 179.60     | 150.67     | 145.78        |

`ostringstream` 对象本身占 366 字节，创建开销很大。声明为 thread_local 之后大幅减少了创建开销，写入速度有 **30 ~ 50%** 提升，虽然比创建固定大小的自定义 LogStream 的性能差些，但是这个速度已经大体上够用，使用 `ostringstream` 省去了自己定义 LogStream 类的繁琐工作。
## 层级时间轮

插入/删除 100 万个随机时间定时器耗时（参数设置见 [benchmark](./testsuite/datetime/benchmark.cc) ）：

```shell
TimerQueue insert/delete 1000000 times: 1602999758ns
TimerWheel insert/delete 1000000 times: 722074328ns
```
## 应用示例
1. HIM聊天示例
