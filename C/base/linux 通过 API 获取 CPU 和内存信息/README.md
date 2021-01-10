一般在查看 CPU 和内存信息时，是通过 `/proc/cpuinfo` 和 `/proc/meminfo` 这两个目录来查看。

还有方法是通过调用系统 API 查看。利用 `sysconf` POSIX 接口，可以获取系统运行时信息，包括 CPU、内存信息，以及进程可以打开最大文件句柄数等。

```C
long sysconf(int name)
```



- `_SC_NPROCESSORS_CONF`: 获取系统中总的 CPU 数量, 注意这里获取的是所有的 CPU 线程的数量
- `_SC_NPROCESSORS_ONLN`: 获取系统中可用的 CPU 数量, 没有被激活的 CPU 则不统计 在内, 例如热添加后还没有激活的.
- `_SC_PHYS_PAGES`: 总的物理内存页大小.
- `_SC_AVPHYS_PAGES`: 可用的物理内存页大小.



代码见 `get_cpu_mem.c` 文件。